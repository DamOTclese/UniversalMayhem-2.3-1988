
/* **********************************************************************
   * create.c	(Main and only module for create).			*
   *									*
   * Copyrite 1988, 1989, 1990, 1991.                                   *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   * Make a universe. This program will put four planets, a star, mine, *
   * and a black hole in each xcord of the universe. This is done by    *
   * storing just the ycord of each item. In this way, to see what's at *
   * xpos 157 and ypos 3047, we read record 157 and see if there is a   *
   * 3047 stored in the record. If so, the position is the determineing *
   * factor as to what that object is.                                  *
   *                                                                    *
   * In addition to the universe file, we also create a planets file    *
   * which is to contain the random number used to determine the cost   *
   * of things, the name of the ship last to visit, the name of the     *
   * ship who's protecting the planet, and the name of the actual       *
   * planet.                                                            *
   *                                                                    *
   * Commerce information can be extracted from the planets file by     *
   * performing the following formula to the file offset:               *
   *                                                                    *
   * index = ((xpos - 1) * 4) + [1...4]                                 *
   *                                                                    *
   * This will return the record number to read from the planets file   *
   * when the ypos is [1...4] planets ypos. The access of the           *
   * information would be performed like:                               *
   *                                                                    *
   * lseek(aplanets, (long) index * sizeof(struct planets_file), 0)     *
   *                                                                    *
   * This will move the file pointer of the fcb to the start of a       *
   * structure record called planets_file. It's not used within this    *
   * application module but it will be used with the rest of Universal  *
   * Mayhem. If the lseek is unable to get the pointer moved, then -1L  *
   * is returned and would need to be checked for.                      *
   *                                                                    *
   *                                                                    *
   * Started:  23/Apr/88                                                *
   * Author:   Fredric L. Rice                                          *
   *                                                                    *
   * 17/Sep/88 - Added one more record to the universe file. It will	*
   *    contain the xposition and y-position of the ion field. In this	*
   *    way, the "center" of the field will never move.			*
   *									*
   ********************************************************************** */

#include "process.h"
#include "string.h"
#include "time.h"
#include "io.h"
#include "conio.h"
#include "stdio.h"
#include "stdlib.h"

#define TRUE             1
#define FALSE            0

/* **********************************************************************
   * Though not really needed, the functions are defined here at the	*
   * top to allow greater prototype checking.				*
   *									*
   ********************************************************************** */

   static short rand_num(void);
   static void config_bad(void);
   static short extract_size(void);
   static void link_black_holes(void);
   static void read_universe(unsigned short uni_record);
   static void write_universe(unsigned short uni_record);
   static FILE *mayhem_fopen(char *fname, char *types, FILE *fpoint);
   static int mayhem_fclose(FILE **fname);
   static int mayhem_fcloseall(void);
   static int mayhem_fread(void *inbuf, size_t tsize, size_t tcount, FILE *fname);
   static int mayhem_fwrite(void *outbuf, size_t tsize, size_t tcount, FILE *fname);

/* **********************************************************************
   * Load in the random number generator module that is used everywhere	*
   * in Universal Mayhem.						*
   *									*
   ********************************************************************** */

#include "random.c"

/* **********************************************************************
   * Place some definitions here.					*
   *									*
   ********************************************************************** */

#define THE_VERSION "2.5"
#define OWNABLE     100
#define NIL	    -1

/* **********************************************************************
   * Define the structure of the universe.				*
   *									*
   * There are one of these for every xsize element as defined in the	*
   * SHIP.CFG file. This makes it interesting... Each record, then,	*
   * must contain the xposition of the universal element as the record	*
   * number comprises the yposition of the universal element.		*
   *									*
   * For instance, if record 3141 contained the following information:	*
   *									*
   * 00121, 07141, 00873, 04003						*
   * 07861								*
   * 02001								*
   * 09912								*
   * 00674								*
   * 									*
   * This describes the following universal elements:			*
   *									*
   * A planet at 3141, 121						*
   * A planet at 3141, 7141						*
   * A planet at 3141, 873						*
   * A planet at 3141, 4003						*
   * A star at 3141, 7861						*
   * A mine at 3141, 2001						*
   * A black hole at 3141, 9912						*
   * A white hole at 3141, 674						*
   *									*
   * Thus when a ship is at 3141, 873, we need only read record number	*
   * 3141, (ships.ship_xpos), and scan the structure to see if any of	*
   * the elements are the same as (ships.ship_ypos). If it were 873,	*
   * we know that the ship is docked.					*
   *									*
   ********************************************************************** */

    struct universe_file {
      short planets[4];       	/* Yposes of four planets in a record        */
      short star;             	/* Ypos of a star on this record of universe */
      short mine;             	/* Ypos of a mine on this record of universe */
      short black_hole;       	/* Ypos of black hole on this record         */
      short white_hole;       	/* Exit xpoint position for blank hole	     */
   } ;

   static struct universe_file universe;

/* **********************************************************************
   * It's important that 'named' be the LAST element of this structure.	*
   * This is because the sysop may optionally have named planets or 	*
   * not. This could save them disk space.				*
   *									*
   ********************************************************************** */

   struct planets_file {
      unsigned char cost;       /* Cost factor to be used to determine costs */
      unsigned char technology; /* Technology level of the planet            */
      char visited;       	/* Last visited by ship name                 */
      char protected;     	/* Planet protected by ship name             */
      char plagued;	     	/* Planet plagued by ship name		     */
      char named[16];        	/* Planets name (placed by a player)         */
   } ;

   static struct planets_file planets;

   static short ysize, xsize;
   static char record[201], to_save;
   static FILE *config, *auniverse, *aplanets;

/* **********************************************************************
   * Here is the main module.						*
   *									*
   ********************************************************************** */

void main(void)
{
   short count;
   long count2;
   unsigned int stime;
   long ltime, would_save;
   char dloop;
   char the_option;
   time_t time_one, time_two;

   config = auniverse = aplanets = (FILE *)NULL;

   ltime = time(NULL);
   stime = ltime / 2;
   srand(stime);

   if ((config = mayhem_fopen("SHIP.CFG", "r", config)) == (FILE *)NULL) {
      config_bad();
      exit(0);
   }

   xsize = extract_size();
   ysize = extract_size();
   mayhem_fclose(&config);

   clrscr();
   textcolor(LIGHTGREEN);

   printf("\n\n  Create program for Universal Mayhem (c) version %s %s",
      THE_VERSION, __DATE__);

   printf("\n    Copyright (c) Fredric Rice. 1988/89/90/91. ALL rights reserved.\n");

   if (xsize < 1000 || xsize > 32766 || ysize < 1000 || ysize > 32766) {
      textcolor(LIGHTRED);
      printf("\nUniverse size is %d by %d - ", xsize, ysize);
      printf("This will NOT work!\n");
      printf("Size must be from 1000 to 32766!\n\n");
      exit(0);
   }

   if ((auniverse = mayhem_fopen("UNIVERSE.DAT", "wb", auniverse)) == (FILE *)NULL) {
      printf("\nUnable to create: UNIVERSE.DAT");
      exit(0);
   }

   would_save = (long) 64L * xsize;
   the_option = 0;

   textcolor(LIGHTBLUE);
   printf("\n\n          1) Allow named planets\n");
   printf("          2) Do NOT allow named planets\n\n");
   printf("If you select option 2, you could save ");
   printf("%ld bytes off file: PLANETS.DAT\n\n", would_save);

   while (the_option < 1 || the_option > 2) {
      printf("Select 1 or 2: ");
      fgets(record, 200, stdin);
      the_option = atoi(record);
   }

   if (the_option == 2) {
      to_save = 16;
   }
   else {
      to_save = 0;
   }

   time(&time_one);

   textcolor(LIGHTGREEN);
   printf("\n      Building [%d] universe records of [%d] bytes each...\n",
      xsize, sizeof(struct universe_file));

   for (count = 0; count < xsize; count++) {
      for (dloop = 0; dloop < 4; dloop++) {
         universe.planets[dloop] = rand_num();
      }

      universe.star = rand_num();
      universe.mine = rand_num();
      universe.black_hole = rand_num();
      universe.white_hole = 0;

      if ((mayhem_fwrite(&universe, sizeof(struct universe_file), 1, auniverse)) != 1) {
         textcolor(LIGHTRED);
         printf("\nError write of UNIVERSE.DAT\n");
         exit(0);
      }
   }

   universe.planets[0] = rand_num();
   universe.planets[1] = rand_num();
   universe.planets[2] = universe.planets[3] = 0;
   universe.star = universe.mine = to_save;
   universe.black_hole = universe.white_hole = 0;

   if ((mayhem_fwrite(&universe, sizeof(struct universe_file), 1, auniverse)) != 1) {
      textcolor(LIGHTRED);
      printf("\nError write of UNIVERSE.DAT\n");
      exit(0);
     }

   mayhem_fclose(&auniverse);
   textcolor(LIGHTBLUE);
   printf("        Universe data has been written.\n");

   if ((aplanets = mayhem_fopen("PLANETS.DAT", "wb", aplanets)) == (FILE *)NULL) {
      printf("\nUnable to create: PLANETS.DAT");
      exit(0);
   }

   printf("          Building [%ld] planets records of [%d] bytes each...",
      (long)xsize * 4, (sizeof(struct planets_file) - to_save));

   planets.visited = planets.protected = planets.plagued = (char)NIL;
   strcpy(planets.named,     "NONE");

   for (count2 = 0; count2 < (long)xsize * 4; count2++) {
      planets.cost = arandom(10L, 35L);
      planets.technology = 100 + rand() / 1000;

      if ((mayhem_fwrite(&planets, sizeof(struct planets_file) - to_save, 1, aplanets)) != 1) {
	 textcolor(LIGHTRED);
         printf("\nError write of PLANETS.DAT\n");
         exit(0);
      }
   }

   count = xsize / 12;

   while (count > 100) {
      count /= 2;
   }

   mayhem_fclose(&aplanets);
   textcolor(LIGHTGREEN);
   printf("\n            Generating linked black holes...");
   link_black_holes();
   printf("              Universal Mayhem Universe has been created.\n");

   time(&time_two);
   printf("                 It took %ld seconds to create\n",
      (long int)difftime(time_two, time_one));

   exit(0);
}

/* **********************************************************************
   * Select a random number from 1 to the size of the ysize of the	*
   * universe. The value is returned to the calling function after      *
   * verification.                                                      *
   *                                                                    *
   ********************************************************************** */

static short rand_num(void)
{
   short the_test;

   the_test = -1;

   while (the_test < 0 || the_test >= ysize) {
      if (ysize < 5)
	 the_test = rand() / 8000;
      else if (ysize < 9)
	 the_test = rand() / 4000;
      else if (ysize < 17)
	 the_test = rand() / 2000;
      else if (ysize < 32)
	 the_test = rand() / 1000;
      else if (ysize < 327)
	 the_test = rand() / 100;
      else if (ysize < 3276)
	 the_test = rand() / 10;
      else
	 the_test = rand();
   }

   return(the_test);
}

/* **********************************************************************
   * The configuration file contained bad information. Either the file  *
   * is not found or the record lengths are a bit unusual, (not         *
   * delimited by a carriage return or something).                      *
   *                                                                    *
   ********************************************************************** */

static void config_bad(void)
{
   printf("\nTrouble reading SHIP.CFG file. Read SHIP.DOC.");
}

/* **********************************************************************
   * Go through record and see where the first space or tab is until    *
   * the end of string is found. When either is, put an end of string   *
   * marker at that location in the string. Then return the integer of  *
   * the value to the function that called it. Because it is possible   *
   * for someone to put the delimiter ; right after the numeric value   *
   * we want, we check for ; as well. We try to make this as bullet     *
   * proof as possible because we don't know the capability of the      *
   * people who are going to run it.                                    *
   *                                                                    *
   * The comments that are allowed after the parameter stored are quite *
   * usefull for people to remember what the various items are for so   *
   * when a change to the file is performed, nothing gets corrupted.    *
   *                                                                    *
   ********************************************************************** */

static short extract_size(void)
{
   char bail_out;
   char count;

   bail_out = FALSE;

   while (! bail_out) {
      if (! feof(config)) {
         fgets(record, 200, config);
	 if (! feof(config)) {
	    if (! feof(config) && record[0] != ';' && strlen(record) > 2) {
	       bail_out = TRUE;
	    }
	 }
	 else {
	    bail_out = TRUE;
	 }
      }
      else {
	 bail_out = TRUE;
      }
   }

   if (feof(config)) {
      fputs("SHIP.CFG is corrupted!\n\r", stderr);
      exit(0);
   }

   for (count = 0; record[count]; count++) {
      if (record[count] == 0x20 || record[count] == 0x9) {
         break;
      }

      if (record[count] == 0x3b) {
         break;
      }
   }

   record[count] = (char)NULL;
   return (atoi(record));
}

/* **********************************************************************
   * Reopen the universe file and develop a link between the black	*
   * holes so that transportation may take place. The entry into a	*
   * hole should cause the exit at another black hole. Perhaps we only	*
   * want to make 75% of the black holes a consistant exit point...	*
   *									*
   ********************************************************************** */

static void link_black_holes(void)
{
   short hole_one, hole_two, link_count, count;

   if ((auniverse = mayhem_fopen("UNIVERSE.DAT", "r+b", auniverse)) == (FILE *)NULL) {
      textcolor(LIGHTRED);
      printf("\nUnable to create: UNIVERSE.DAT");
      exit(0);
   }

   link_count = xsize / 12;

   for (count = 0; count < link_count; count++) {

try_one_random_again:
      hole_one = arandom(1L, xsize - 1);
      read_universe((unsigned short) hole_one);

      if (universe.white_hole > 0) {
         goto try_one_random_again;
      }

try_two_random_again:
      hole_two = arandom(1L, xsize - 1);
      read_universe((unsigned short) hole_two);

      if (universe.white_hole > 0) {
         goto try_two_random_again;
      }

      universe.white_hole = hole_one;
      write_universe((unsigned short) hole_two);

      read_universe((unsigned short) hole_one);
      universe.white_hole = hole_two;
      write_universe((unsigned short) hole_one);
   }
   mayhem_fcloseall();
   printf("%d links made\n", link_count);
}

/* **********************************************************************
   * Read in a record from the universe file.				*
   *									*
   ********************************************************************** */

static void read_universe(unsigned short uni_record)
{
   if (fseek(auniverse, ((long) uni_record * sizeof(struct universe_file)), 0) != 0) {
      textcolor(LIGHTRED);
      printf("\nUnable to point to read universe record.");
      printf("\nTried to point to: %ld, (record %d)",
	 ((long) uni_record * sizeof(struct universe_file)),
	 uni_record);
      mayhem_fcloseall();
      exit(0);
   }

   if ((mayhem_fread(&universe, sizeof(struct universe_file), 1, auniverse)) != 1) {
      textcolor(LIGHTRED);
      printf("\nError occured when reading universe record.\n");
      printf("It was universe record number %d\n", uni_record);
      printf("Dpoint is %ld\n",
	  ((long) uni_record * sizeof(struct universe_file)));
      printf("Size of universe is %d\n", sizeof(struct universe_file));
      mayhem_fcloseall();
      exit(0);
   }
}

/* **********************************************************************
   * Write a record to the universe file.				*
   *									*
   ********************************************************************** */

static void write_universe(unsigned short uni_record)
{
   if (fseek(auniverse, ((long) uni_record * sizeof(struct universe_file)), 0) != 0) {
      textcolor(LIGHTRED);
      printf("\nUnable to point to write universe record.");
      printf("\nTried to point to: %ld",
         ((long) uni_record * sizeof(struct universe_file)));
      mayhem_fcloseall();
      exit(0);
   }

   if ((mayhem_fwrite(&universe, sizeof(struct universe_file), 1, auniverse)) != 1) {
      textcolor(LIGHTRED);
      printf("\nError occured when writeing universe record.\n");
      printf("Tried to write record number %d", uni_record);
      mayhem_fcloseall();
      exit(0);
   }
}

/* **********************************************************************
   * Check to see if the file is already open. If so, report a log and	*
   * return the existing pointer.					*
   *									*
   ********************************************************************** */

static FILE *mayhem_fopen(char *fname, char *types, FILE *fpoint)
{
   if (fpoint != (FILE *)NULL) {
      return(fpoint);
   }

   return(fopen(fname, types));
}

/* **********************************************************************
   * Check to see if the file is already closed. If so, report an error	*
   * and return an end of file condition.				*
   *									*
   ********************************************************************** */

static int mayhem_fclose(FILE **fname)
{
   int result;

   if (*fname == (FILE *)NULL) {
      return(EOF);
   }

   if ((result = fclose(*fname)) == 0) {
      *fname = (FILE *)NULL;
   }

   return(result);
}

/* **********************************************************************
   * Close all files. Return stdio's return value.			*
   *									*
   ********************************************************************** */

static int mayhem_fcloseall(void)
{
   mayhem_fclose(&config);
   mayhem_fclose(&auniverse);
   mayhem_fclose(&aplanets);
   return(0);
}

/* **********************************************************************
   * Make a read. Before this is done, however, make sure that the file	*
   * has been opened. If not, report a failure and return 0 bytes read.	*
   *									*
   ********************************************************************** */

static int mayhem_fread(void *inbuf, size_t tsize, size_t tcount, FILE *fname)
{
   if (fname == (FILE *)NULL) {
      return(0);
   }

   return(fread(inbuf, tsize, tcount, fname));
}

/* **********************************************************************
   * Make a write. Before this is done, however, make sure that the 	*
   * file has been opened. If not, report an error and then return 0 as	*
   * the number of bytes written.					*
   *									*
   ********************************************************************** */

static int mayhem_fwrite(void *outbuf, size_t tsize, size_t tcount, FILE *fname)
{
   if (fname == (FILE *)NULL) {
      return(0);
   }

   return(fwrite(outbuf, tsize, tcount, fname));
}

