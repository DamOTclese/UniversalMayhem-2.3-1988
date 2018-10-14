/***********************************************************************

                ASYNCHRONOUS I/O ROUTINES - HEADER FILE
                         Version 1.00, 12/24/87

The following are data and procedure definitions for programs which use
the ASYNC.C modeule.

Written and placed in the public domain by:

			   Glen F. Marshall
			   1006 Gwilym Circle
			   Berwyn, PA 19312

***********************************************************************/

typedef unsigned int ComPort;		/* Asynchronous ports */
	#define Com1 0
	#define Com2 1

typedef unsigned int ComSpeed;		/* Baud rate */
	#define LowSpeed  0		/* 300 */
	#define MedSpeed  1		/* 1200 */
	#define HighSpeed 2		/* 2400 */

typedef unsigned int ComParity; 	/* No/Even/Odd parity */
	#define NoParity   0
	#define EvenParity 1
	#define OddParity  2

typedef unsigned int ComEcho;		/* Input echo option */
	#define NoEcho 0
	#define Echo   1

/* This clears the input buffer */
void ComInFlush(ComPort);

/* This clears the output buffer */
void ComOutFlush(ComPort);

/* This procedure MUST be called before doing any I/O. */
void ComOpen(ComPort, ComSpeed, ComParity);

/* This shuts-down communications */
void ComClose(ComPort);

/* This tests if there is a connection to write to. */
int ComOutReady(ComPort);

/* This writes output, filling the ring buffer if necessary. */
void ComOut(ComPort, unsigned char);

/* The writes an output string. */
void ComOutStr(ComPort, char *);

/* This tells you if there's any input */
int ComInReady(ComPort);

/* This tells you if there's been no input for too many seconds. */
int ComTimeout(ComPort, int);

/* This reads input from the buffer */
int ComIn(ComPort, char *, ComEcho);

/* This reads an input string terminated by CR, size, or timeout. */
void ComInStr(ComPort, char *, int, ComEcho);
