/***********************************************************************

		       ASYNCHRONOUS I/O ROUTINES
			 Version 1.00, 12/24/87

The following are data definitions and procedures which perform I/O
functions for IBM PC asynchronous communications adapters.  Comments
here and there, below, explain what's what.

Although these routines are set-up to support COM1 and COM2, which may
be open simultaneously if you wish, it's not much of an effort to make
them support more.  Also, it's a trivial excercise to add support for up
to 9600 baud - presuming the machine is fast enough.

Conditional compiler directives here and there account for differences
between TurboC 1.0 and Microsoft C 5.0.

Written and placed in the public domain by:

			   Glen F. Marshall
			   1006 Gwilym Circle
			   Berwyn, PA 19312

***********************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __TURBOC__
	#include <dos.h>
	#define _disable	disable
	#define _enable 	enable
	#define _dos_getvect	getvect
	#define _dos_setvect	setvect
	#include <alloc.h>
	#define _fmalloc	farmalloc
	#define _ffree		farfree
#else
	#include <dos.h>
	#define MK_FP(seg,ofs)	((void far *)(((unsigned long)(seg)<<16)|(ofs)))
	#define peek(a,b)	(*((int far*)MK_FP((a),(b))))
	#include <conio.h>
	#include <malloc.h>
#endif


#include "async.h"

    extern unsigned long buffered;
    static char flip_flop = 0;
    static char in_interrupt = 0;
    unsigned long recursed = 0;
    unsigned long over_run = 0;
    unsigned long transmit_overflow = 0;
    unsigned long missed_tx = 0;
    extern char *record;
    extern char port_assignment;
    static char far *video_pointer;

#define ComData ComPortAddr	/* Communications port data xmit/recv */

#define ComEnable ComPortAddr+1 /* Communications port interrupt enable */
	#define ComEnableRD 1	/* data received */
	#define ComEnableTX 2	/* transmit register empty */
	#define ComEnableLS 4	/* line status */
	#define ComEnableMS 8	/* modem status */

#define ComIdent ComPortAddr+2	/* Communications port interrupt identity */
        #define ComIdentNot 1   /* interrupt not pending */
	#define ComIdentMS  0	/* modem status interrupt */
	#define ComIdentTX  2	/* transmit register empty */
	#define ComIdentRD  4	/* data received */
	#define ComIdentLS  6	/* line status interrupt */

#define ComLineCtl ComPortAddr+3 /* Communications port line control */
	#define ComLineBreak  64 /* Communications Line Send Break */
	#define ComLineDLAB  128 /* Communications Divisor Latch Access Bit */

#define ComModemCtl ComPortAddr+4 /* Communications port modem control */
	#define ComModemDTR  1	/* data terminal ready */
	#define ComModemRTS  2	/* request to send */
	#define ComModemOUT1 4	/* Out 1 signal (enables ring) */
	#define ComModemOUT2 8	/* Out 2 signal (enables data receive) */

#define ComLineStat ComPortAddr+5 /* Communications port line status */
	#define ComLineOR    2	/* overrun */
	#define ComLinePE    4	/* parity error */
	#define ComLineFE    8	/* framing error */
	#define ComLineBI   16	/* break interrupt */
	#define ComLineTHRE 32	/* transmit holding register empty */

#define ComModemStat ComPortAddr+6 /* Communications port modem status */
	#define ComModemCTS  16 /* clear to send */
	#define ComModemDSR  32 /* data set ready */
	#define ComModemRI   64 /* phone ring */
	#define ComModemCD  128 /* carrier detect */

/* Communications port baud-rate divisors */
	#define ComDivLo ComPortAddr
	#define ComDivHi ComPortAddr+1

#define Cmd8259 0x20		/* 8259 interrupt controller command port */
	#define EOI8259 0x20	/* "End Of Interrupt" command */
	#define RIL8259 0x0B	/* "Report Interrupt Level" command */

#define Msk8259 0x21		/* 8259 interrupt controller mask port */

#define ComTimeLimit 120	/* Standard number of seconds till timeout */

/* A "byte" is an unsigned character */
	#define byte unsigned char

/* Input and output buffer definitions: MUST be power of two in size */
	typedef byte ComInBuffer[4096];
        #define ComInBufferTop (sizeof(ComInBuffer)-1)
	typedef byte ComOutBuffer[4096];
        #define ComOutBufferTop (sizeof(ComInBuffer)-1)

/* This helps keep internal workings fast */
	#define internal static void near pascal

struct	ComCtlRec {
	int		ComPortAddr;		/* base device port */
	ComSpeed	ComCtlSpeed;		/* current baud rate */
	ComParity	ComCtlParity;		/* current parity */
	byte		LastComIdent;		/* interrupt reason */
	byte		LastComLineStat;	/* line status */
	byte		LastComModemStat;	/* modem status */
#ifdef __TURBOC__
	void interrupt	(*ComSavedVect)();	/* saved interrupt vector */
#else
	void (interrupt far *ComSavedVect)();	/* saved interrupt vector */
#endif
	int		ComInInIX;		/* ring buffer index */
	int		ComInOutIX;		/* ring buffer index */
	int		ComOutOutIX;		/* ring buffer index */
	int		ComOutInIX;		/* ring buffer index */
	ComInBuffer	ComInBuf;		/* input ring buffer */
	ComOutBuffer	ComOutBuf;		/* output ring buffer */
	};

typedef struct ComCtlRec far *ComCtlRecPtr;

static ComCtlRecPtr	ComPtr[2];		/* Ptr, port control */

static const byte	ComMsk8259[2] = {16,8}; /* 8259 interrupt mask */


/* Communications interrupt handler */
#ifdef __TURBOC__
static void interrupt AsyncInterrupt()
#else
static void interrupt far AsyncInterrupt()
#endif
{
    ComCtlRecPtr P;

    _enable();
    if (in_interrupt == 1) return;
    in_interrupt = 1;

    outp(Cmd8259,RIL8259); /* which COM port interrupted? */

    if (flip_flop == 0) {
        *video_pointer = '\\';
        flip_flop = 1;
    }
    else {
        *video_pointer = '/';
        flip_flop = 0;
    }

    if (inp(Cmd8259) & ComMsk8259[Com1])
        P = ComPtr[Com1];
    else
        P = ComPtr[Com2];

    P->LastComIdent = inp(P->ComIdent);

hit_it_again:
    if ((P->LastComIdent & ComIdentRD) == ComIdentRD) {
        if (((P->ComInInIX+1) & ComInBufferTop) != P->ComInOutIX) {
            P->ComInBuf[P->ComInInIX] = inp(P->ComData);
            P->ComInInIX++;
            P->ComInInIX &= ComInBufferTop;
        }
        else { /* buffer overrun */
            P->LastComLineStat |= ComLineOR;
            over_run++;
        }
    }

    if ((P->LastComIdent & ComIdentTX) == ComIdentTX) {
        if (P->ComOutOutIX != P->ComOutInIX) {
            outp(P->ComData, P->ComOutBuf[P->ComOutOutIX]);
            P->ComOutOutIX++;
        }
        else {
            P->ComOutOutIX = P->ComOutInIX = 0;
        }
    }

    if ((P->LastComIdent & ComIdentLS) == ComIdentLS) {
        P->LastComLineStat = inp(P->ComLineStat);
    }

    if ((P->LastComIdent & ComIdentMS) == ComIdentMS) {
        P->LastComModemStat = inp(P->ComModemStat);
    }

    P->LastComIdent = inp(P->ComIdent);

    if ((P->LastComIdent & ComIdentNot) != ComIdentNot) {
        recursed++;
        goto hit_it_again;
    }

    if ((inp(P->ComLineStat) & ComLineTHRE) != 0) {
        if (P->ComOutOutIX != P->ComOutInIX) {
            outp(P->ComData, P->ComOutBuf[P->ComOutOutIX]);
            P->ComOutOutIX++;
            missed_tx++;
        }
    }

    in_interrupt = 0;
    outp(Cmd8259,EOI8259); /* let the 8259 continue */
}

/* This turns the communications interrupts on or off */
internal ComSet8259(ComPort ComDev, byte Sw)
{
	_disable();

        if (Sw)
		outp(Msk8259,inp(Msk8259)&(~ComMsk8259[ComDev]));
	else
		outp(Msk8259,inp(Msk8259)|ComMsk8259[ComDev]);

        _enable();
}

/* This establishes the communications device base port address */
internal ComSetPort(ComPort ComDev)
{
	ComPtr[ComDev]->ComPortAddr = peek(0x40,ComDev<<1);
}

/* This sets the communications baud rate */
internal ComSetSpeed(ComPort ComDev, ComSpeed ComRate)
{
	static const int ComBaudDiv[3] = {384,96,48}; /* 300/1200/2400 baud */
#ifndef __TURBOC__
	register unsigned int _CX;
	#define _CL ((unsigned char)(_CX & 0xFF))
	#define _CH ((unsigned char)(_CX >> 8))
#endif
	ComCtlRecPtr P = ComPtr[ComDev];
	_disable();
	P->ComCtlSpeed	= ComRate;
	outp(P->ComLineCtl,inp(P->ComLineCtl)|ComLineDLAB);
	_CX = ComBaudDiv[ComRate];
	outp(P->ComDivLo,_CL);
	outp(P->ComDivHi,_CH);
	outp(P->ComLineCtl,inp(P->ComLineCtl)&(~ComLineDLAB));
	_enable();
}

/* This modifies the line control register for parity & data bits */
internal ComSetFormat(ComPort ComDev, ComParity ComFormat)
{
        static const byte ComLineInit[3] = {3,26,10};                              
	ComCtlRecPtr P = ComPtr[ComDev];
	_disable();
	P->ComCtlParity = ComFormat;
        outp(P->ComLineCtl,ComLineInit[ComFormat]);
        bioscom(0, 0x02 | 0x00 | 0x18 | 0x80, ComDev);
        _enable();
}

/* This modifies the communications interrupt enable register */
internal ComSetEnable(ComPort ComDev, byte Enable)
{
	_disable();
	outp(ComPtr[ComDev]->ComEnable,Enable);
	_enable();
}

/* This modifies the modem control register for DTR, RTS, etc. */
internal ComSetModem(ComPort ComDev, byte ModemCtl)
{
	_disable();
	outp(ComPtr[ComDev]->ComModemCtl,ModemCtl);
	_enable();
}

/* This clears the input buffer */
void ComInFlush(ComPort ComDev)
{
	ComCtlRecPtr P = ComPtr[ComDev];
	P->ComInInIX  = 0;
	P->ComInOutIX = 0;
}

/* This clears the output buffer */
void ComOutFlush(ComPort ComDev)
{
	ComCtlRecPtr P = ComPtr[ComDev];
	P->ComOutOutIX = 0;
	P->ComOutInIX  = 0;
}

/* This modifies the communications interrupt vector */
internal ComSetInterrupt(ComPort ComDev, byte Sw)
{
	static const int ComInt[2] = {12,11};	/* async interrupt # */
	ComCtlRecPtr P = ComPtr[ComDev];
	if (Sw)
	{
		P->ComSavedVect = _dos_getvect(ComInt[ComDev]);
		_dos_setvect(ComInt[ComDev],AsyncInterrupt);
	}
	else
	{
		_dos_setvect(ComInt[ComDev],P->ComSavedVect);
	}
}

/* This procedure MUST be called before doing any I/O. */
void ComOpen(ComPort ComDev, ComSpeed ComRate, ComParity ComFormat)
{
	ComCtlRecPtr P = _fmalloc(sizeof(struct ComCtlRec));

        video_pointer = (char *)MK_FP(0xb000, 0x0);

        P->ComInInIX = 0;
        P->ComInOutIX = 0;
        P->ComOutOutIX = 0;
        P->ComOutInIX = 0;

        ComPtr[ComDev] = P;
	ComSet8259(ComDev,0);
	ComSetPort(ComDev);
/*
        ComSetSpeed(ComDev,ComRate);
	ComSetFormat(ComDev,ComFormat);
*/
	ComSetEnable(ComDev,ComEnableRD+ComEnableTX+ComEnableLS+ComEnableMS);
	ComSetModem(ComDev,ComModemDTR+ComModemRTS+ComModemOUT1+ComModemOUT2);
	ComInFlush(ComDev);
	ComOutFlush(ComDev);
	ComSetInterrupt(ComDev,1);
	ComSet8259(ComDev,1);
	P->LastComIdent = 1;
	P->LastComLineStat  = inp(P->ComLineStat);
	P->LastComModemStat = inp(P->ComModemStat);
        if (ComFormat == 0 && ComRate == 0) {
                /* */
        }
}

/* This shuts-down communications */
void ComClose(ComPort ComDev)
{
	ComCtlRecPtr P = ComPtr[ComDev];
	ComSet8259(ComDev,0);
	ComSetEnable(ComDev,0);
	ComSetInterrupt(ComDev,0);
/*
        ComSetModem(ComDev,0);
        outp(P->ComLineCtl,0);
*/
	_ffree(P);
	ComPtr[ComDev] = NULL;
}

/* This tests if there is a connection to write to. */
int ComOutReady(ComPort ComDev)
{
	return((ComPtr[ComDev]->LastComModemStat & ComModemCTS) != 0);
}

void empty_transmit_buffer(ComPort ComDev)
{
    ComCtlRecPtr P = ComPtr[ComDev];

    _disable();

    while (P->ComOutOutIX != P->ComOutInIX) {
        if ((inp(P->ComLineStat) & ComLineTHRE) != 0) {
            outp(P->ComData, P->ComOutBuf[P->ComOutOutIX]);
            P->ComOutOutIX++;
        }
    }

    P->ComOutOutIX = P->ComOutInIX = 0;
    _enable();
}

/* This writes output, filling the ring buffer if necessary. */
void ComOut(ComPort ComDev, byte X)
{
    char incrimented_overflow = 0;
    ComCtlRecPtr P = ComPtr[ComDev];

    if (P->ComCtlParity != NoParity)
        X &= 0x7F;

    if (((inp(P->ComLineStat) & ComLineTHRE) != 0) &&
        (P->ComOutInIX == P->ComOutOutIX))
    {
        outp(P->ComData,X);
    }
    else
    {
        if (P->ComOutInIX >= (ComOutBufferTop - 1000)) {
            if (! incrimented_overflow) {
                transmit_overflow++;
                incrimented_overflow = 1;
            }

            empty_transmit_buffer(ComDev);
        }

        P->ComOutBuf[P->ComOutInIX] = X;
        P->ComOutInIX++;
        buffered++;
    }
}


/* The writes an output string. */
void ComOutStr(ComPort ComDev, char *Str)
{
        while(*Str) {
            putchar(*Str);
            ComOut(ComDev, *(Str++));
        }
}

/* This tells you if there's any input */
int ComInReady(ComPort ComDev)
{
	ComCtlRecPtr P = ComPtr[ComDev];
	return(P->ComInInIX != P->ComInOutIX);
}

/* This tells you if there's been no input for too many seconds. */
int ComTimeout(ComPort ComDev, int TimeLimit)
{
	register unsigned int I;
	while(TimeLimit-- > 0)
	{
                for (I=1; I<=100; I++)
		{
#ifdef __TURBOC__
			if (ComInReady(ComDev)) return(0);
			_AH = 0x2C;
			geninterrupt(0x21);
			_BL = _DL;
			do
			{
				_AH = 0x2C;
				geninterrupt(0x21);
			}
			while(_BL != _DL);
#else
			union REGS regs;
			unsigned char DL;
			if (ComInReady(ComDev)) return(0);
			regs.h.ah = 0x2C;
			intdos(&regs,&regs);
			DL = regs.h.dl;
			do
			{
				regs.h.ah = 0x2C;
				intdos(&regs,&regs);
			}
			while(DL != regs.h.dl);
#endif
		}
	}
	return(1);
}

/* This reads input from the ring buffer */
int ComIn(ComPort ComDev, char *X, ComEcho EchoOpt)
{
	ComCtlRecPtr P = ComPtr[ComDev];
	if (ComInReady(ComDev))
	{
		*X = P->ComInBuf[(P->ComInOutIX)++];
		P->ComInOutIX &= ComInBufferTop;
		if (EchoOpt)
		{
			ComOut(ComDev,*X);
			if (*X == 13)
				ComOut(ComDev,10);
		}
		return(1);
	}
	else
	{
		return(0);
	}
}

/* This reads an input string terminated by CR, size, or timeout. */
void ComInStr(ComPort ComDev, char *X, int MaxLen, ComEcho EchoOpt)
{
	register int L = 0;
	while(L++ < MaxLen)
	{
		if (ComTimeout(ComDev,ComTimeLimit)) break;
		ComIn(ComDev,X,EchoOpt);
		if (*(X++) == '\r') break;
	}
	*X = '\0';
}

