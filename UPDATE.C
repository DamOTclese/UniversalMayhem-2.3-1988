
/* **********************************************************************
   * update.c                                                           *
   *                                                                    *
   * Copyright 1989, 1990, 1991.                                        *
   * Fredric L. Rice. All rights reserved.                              *
   *                                                                    *
   * This stand-along program is a utility which will convert the older	*
   * Universal Mayhem SHIP.DAT player data files to the newer format	*
   * of Universal Mayhem.						*
   *									*
   ********************************************************************** */

#include "defines.h"
#include "planets.h"
#include "goal.h"
#include "stdio.h"
#include "ctype.h"
#include "conio.h"
#include "process.h"
#include "stdlib.h"
#include "string.h"
#include "alloc.h"

/* **********************************************************************
   * Definitions to make this puppy work.				*
   *									*
   ********************************************************************** */

/* **********************************************************************
   * Define the prototypes used here.					*
   *									*
   ********************************************************************** */

   static char read_old_ship_format(short record_number);
   static char write_new_ship_format(short record_number);
   static char record[201];

   FILE *mayhem_fopen(char *fname, char *types, FILE *fpoint);
   int mayhem_fclose(FILE **fname);
   int mayhem_fcloseall(void);
   int mayhem_fread(void *inbuf, size_t tsize, size_t tcount, FILE *fname);
   int mayhem_fwrite(void *outbuf, size_t tsize, size_t tcount, FILE *fname);

/* **********************************************************************
   * Define the format of the older version of the players data file.	*
   *									*
   ********************************************************************** */

struct old_ship_file {      /* Structure of SHIP.DAT file                 */
  long ship_xpos;           /* The record number (xpos) of where ship is  */
  long ship_ypos;           /* The ypos of where the ship is              */
  UC ship_universe;         /* The universe the ship is in.               */
  UL ship_power;            /* The amount of power in ships engines       */
  long ship_shield;         /* The amount of power in ships shields       */
  UL ship_credits;          /* The amount of money in the ship            */
  short ship_cargo;         /* The amount of cargo in the ship            */
  short ship_crew;          /* The number of crew members in the ship     */
  short ship_shuttle;       /* The class shuttle craft in the ship        */
  short ship_hull;          /* The class hull the ship is                 */
  short ship_cloak;         /* The class cloaking device the ship has     */
  short ship_sensor;        /* The class sensor the ship has              */
  char ship_name[5];        /* The name of the ship everyone sees         */
  char ship_pass[21];       /* Password for getting into captains chair   */
  char ship_date[27];       /* Date last signed in                        */
  short ship_warp;          /* Class warp drive the ship has              */
  short ship_torp;          /* Number of torps the ship contains          */
  long base_xpos;           /* The record number (xpos) of ships base     */
  long base_ypos;           /* The ypos of the ships resupply base        */
  UC base_universe;         /* The universe the base is in                */
  UL base_credits;          /* The amount of money in the supply base     */
  UL base_cargo;            /* The amount of cargo in the supply base     */
  UL base_crew;             /* The number of crew members in base         */
  short base_cloak;         /* The class cloak of the base                */
  long base_shield;         /* The number of energy units in base shield  */
  char ship_person[31];     /* The personal name of the captain of ship   */
  short rem_xpos[10];       /* Record numbers (yposes) of remotes         */
  short rem_ypos[10];       /* xposes of four remote robot sensors        */
  UC rem_universe[10];      /* The universe the remote is in              */
  short total_kills;        /* The number of times this ship has killed   */
  short time_remaining;     /* Number of minutes remaining to play        */
  short planets_owned;      /* Number of planets captain has named        */
  short plague_flag;        /* 0 is no plague, else greater than 0.       */
  short ship_infected;      /* Is ship infected? Holds class infection    */
  short ship_morale;        /* Morale factor from 0% to 100%              */
  char allies[5][5];        /* Up to 5 ships to be allied with names      */
  float taxes;              /* Accumulative taxes for Galactic Police.    */
  char tax_warnings;        /* Number of warnings on taxes / Auction flg  */
  short attack_sleds;       /* Number of attack sleds on board            */
  short sled_xpos[15];      /* Xposition of up to 15 attack sleds         */
  short sled_ypos[15];      /* Yposition of up to 15 attack sleds         */
  UC sled_universe[15];     /* The universe the sled is in                */
  short sled_swarm[15];     /* Number of ships in up to 15 attack swarms   */
  short sled_power[15];     /* Average power available for the swarms     */
  char last_at_by[5];       /* Name of ship that last attacked this ship  */
  char last_at_who[5];      /* Name of ship this ship last attacked       */
  char last_at_status;      /* Status of the last ship attacked           */
  short last_torp_count;    /* number of torps by last attacked by        */
  float last_phaser_count;  /* Number of millions of units last atkd by   */
  short log_count;          /* Number of times captain has signed in      */
  char base_death[5];       /* Who destroyed this ships base?             */
  short base_hit_count;     /* Number of times base hit while unshielded  */
  long x_planet_info[10];   /* X-Position of up to ten planets or points  */
  long y_planet_info[10];   /* Y-Position of up to ten planets or points  */
  char outstanding_bid;     /* Ships number bidded on                     */
  long bid_amount;          /* Amount of the bid                          */
  char base_boarded[5];     /* Allied ship that last boarded ships base   */
  float planet_taxes[10];   /* Taxes owed at ten planets.                 */
  long tax_xpos[10];        /* Xpositions of the ten tax due planets      */
  long tax_ypos[10];        /* Ypositions of the ten tax due planets      */
  char slot_owned[10][5];   /* Name of the ship that owns taxed planet    */
  short owned_planets[OWNABLE]; /* Xpos of 1 of 4 possible ypos of owned  */
  unsigned allow_colors : 1;/* Set to either 'Y' or 'N' for ansi colors   */
  short sick_bay;           /* Number of crew members in sick bay.        */
  long bounty;              /* Number of credits available for bounty     */
  char who_destroyed[5];    /* Name of ship that destroyed this ship.     */
  short scout_xpos[10];     /* Xpos of the scouts                         */
  short scout_ypos[10];     /* Ypos of the scouts                         */
  short scout_to_x[10];     /* Xpos of where to warp to                   */
  short scout_to_y[10];     /* Ypos of where to warp to                   */
  char scout_direction[10]; /* 0-Outbound, 1-Station keeping , 2-Inbound  */
  UC scout_universe[10];    /* What universe is the scout in?             */
  char stay_on_station[10]; /* Will it stay on station? No? It'll return. */
  char pirate_code[30];     /* The pirate access code                     */
  char leashed_by;          /* Ship this one is leased by                 */
  char leashed_to;          /* Ship this one has got a leash on           */
  char local;               /* 0 if local, 1 if imported                  */
} ;                         /* active ship                                */

struct old_ship_file old_ship;

/* **********************************************************************
   * Define the format of the newer version of the players data file.	*
   *									*
   ********************************************************************** */

struct new_ship_file {      /* Structure of SHIP.DAT file                 */
  long ship_xpos;           /* The record number (xpos) of where ship is  */
  long ship_ypos;           /* The ypos of where the ship is              */
  UC ship_universe;         /* The universe the ship is in.               */
  UL ship_power;            /* The amount of power in ships engines       */
  long ship_shield;         /* The amount of power in ships shields       */
  UL ship_credits;          /* The amount of money in the ship            */
  short ship_cargo;         /* The amount of cargo in the ship            */
  short ship_crew;          /* The number of crew members in the ship     */
  short ship_shuttle;       /* The class shuttle craft in the ship        */
  short ship_hull;          /* The class hull the ship is                 */
  short ship_cloak;         /* The class cloaking device the ship has     */
  short ship_sensor;        /* The class sensor the ship has              */
  char ship_name[5];        /* The name of the ship everyone sees         */
  char ship_pass[21];       /* Password for getting into captains chair   */
  char ship_date[27];       /* Date last signed in                        */
  short ship_warp;          /* Class warp drive the ship has              */
  short ship_torp;          /* Number of torps the ship contains          */
  long base_xpos;           /* The record number (xpos) of ships base     */
  long base_ypos;           /* The ypos of the ships resupply base        */
  UC base_universe;         /* The universe the base is in                */
  UL base_credits;          /* The amount of money in the supply base     */
  UL base_cargo;            /* The amount of cargo in the supply base     */
  UL base_crew;             /* The number of crew members in base         */
  short base_cloak;         /* The class cloak of the base                */
  long base_shield;         /* The number of energy units in base shield  */
  char ship_person[31];     /* The personal name of the captain of ship   */
  short rem_xpos[10];       /* Record numbers (yposes) of remotes         */
  short rem_ypos[10];       /* xposes of four remote robot sensors        */
  UC rem_universe[10];      /* The universe the remote is in              */
  short total_kills;        /* The number of times this ship has killed   */
  short time_remaining;     /* Number of minutes remaining to play        */
  short planets_owned;      /* Number of planets captain has named        */
  short plague_flag;        /* 0 is no plague, else greater than 0.       */
  short ship_infected;      /* Is ship infected? Holds class infection    */
  short ship_morale;        /* Morale factor from 0% to 100%              */
  char allies[5][5];        /* Up to 5 ships to be allied with names      */
  float taxes;              /* Accumulative taxes for Galactic Police.    */
  char tax_warnings;        /* Number of warnings on taxes / Auction flg  */
  short attack_sleds;       /* Number of attack sleds on board            */
  short sled_xpos[15];      /* Xposition of up to 15 attack sleds         */
  short sled_ypos[15];      /* Yposition of up to 15 attack sleds         */
  UC sled_universe[15];     /* The universe the sled is in                */
  short sled_swarm[15];     /* Number of ships in up to 15 attack swarms  */
  short sled_power[15];     /* Average power available for the swarms     */
  char last_at_by[5];       /* Name of ship that last attacked this ship  */
  char last_at_who[5];      /* Name of ship this ship last attacked       */
  char last_at_status;      /* Status of the last ship attacked           */
  short last_torp_count;    /* number of torps by last attacked by        */
  float last_phaser_count;  /* Number of millions of units last atkd by   */
  short log_count;          /* Number of times captain has signed in      */
  char base_death[5];       /* Who destroyed this ships base?             */
  short base_hit_count;     /* Number of times base hit while unshielded  */
  long x_planet_info[10];   /* X-Position of up to ten planets or points  */
  long y_planet_info[10];   /* Y-Position of up to ten planets or points  */
  char outstanding_bid;     /* Ships number bidded on                     */
  long bid_amount;          /* Amount of the bid                          */
  char base_boarded[5];     /* Allied ship that last boarded ships base   */
  float planet_taxes[10];   /* Taxes owed at ten planets.                 */
  long tax_xpos[10];        /* Xpositions of the ten tax due planets      */
  long tax_ypos[10];        /* Ypositions of the ten tax due planets      */
  char slot_owned[10][5];   /* Name of the ship that owns taxed planet    */
  short owned_planets[OWNABLE]; /* Xpos of 1 of 4 possible ypos of owned  */
  unsigned allow_colors : 1;/* Set to either 'Y' or 'N' for ansi colors   */
  short sick_bay;           /* Number of crew members in sick bay.        */
  long bounty;              /* Number of credits available for bounty     */
  char who_destroyed[5];    /* Name of ship that destroyed this ship.     */
  short scout_xpos[10];     /* Xpos of the scouts                         */
  short scout_ypos[10];     /* Ypos of the scouts                         */
  short scout_to_x[10];     /* Xpos of where to warp to                   */
  short scout_to_y[10];     /* Ypos of where to warp to                   */
  char scout_direction[10]; /* 0-Outbound, 1-Station keeping , 2-Inbound  */
  UC scout_universe[10];    /* What universe is the scout in?             */
  char stay_on_station[10]; /* Will it stay on station? No? It'll return. */
  char pirate_code[30];     /* The pirate access code                     */
  char leashed_by;          /* Ship this one is leased by                 */
  char leashed_to;          /* Ship this one has got a leash on           */
  char local;               /* 0 if local, 1 if imported                  */
  char point_tag[20][21];   /* Symbolic name of x/y posistion tag name.   */
  char macro[MACROS][201];  /* Keyboard Macros.                           */
} ;

struct new_ship_file new_ship;

/* **********************************************************************
   * Define global data storage.					*
   *									*
   ********************************************************************** */

   static FILE *ship_in_file, *ship_out_file;
   static FILE *configuration_file;
   static short total_players = 0;

static void insert_new(void)
{
   char count;

   new_ship.ship_xpos = old_ship.ship_xpos;
   new_ship.ship_ypos = old_ship.ship_ypos;
   new_ship.ship_universe = old_ship.ship_universe;
   new_ship.ship_power = old_ship.ship_power;
   new_ship.ship_shield = old_ship.ship_shield;
   new_ship.ship_credits = old_ship.ship_credits;
   new_ship.ship_cargo = old_ship.ship_cargo;
   new_ship.ship_crew = old_ship.ship_crew;
   new_ship.ship_shuttle = old_ship.ship_shuttle;
   new_ship.ship_hull = old_ship.ship_hull;
   new_ship.ship_cloak = old_ship.ship_cloak;
   new_ship.ship_sensor = old_ship.ship_sensor;
   strcpy(new_ship.ship_name, old_ship.ship_name);
   strcpy(new_ship.ship_pass, old_ship.ship_pass);
   strcpy(new_ship.ship_date, old_ship.ship_date);
   new_ship.ship_warp = old_ship.ship_warp;
   new_ship.ship_torp = old_ship.ship_torp;
   new_ship.base_xpos = old_ship.base_xpos;
   new_ship.base_ypos = old_ship.base_ypos;
   new_ship.base_universe = old_ship.base_universe;
   new_ship.base_credits = old_ship.base_credits;
   new_ship.base_cargo = old_ship.base_cargo;
   new_ship.base_crew = old_ship.base_crew;
   new_ship.base_cloak = old_ship.base_cloak;
   new_ship.base_shield = old_ship.base_shield;
   strcpy(new_ship.ship_person, old_ship.ship_person);

   for (count = 0; count < 10; count++) {
      new_ship.rem_xpos[count] = old_ship.rem_xpos[count];
      new_ship.rem_ypos[count] = old_ship.rem_ypos[count];
      new_ship.rem_universe[count] = old_ship.rem_universe[count];
   }

   new_ship.total_kills = old_ship.total_kills;
   new_ship.time_remaining = old_ship.time_remaining;
   new_ship.planets_owned = old_ship.planets_owned;
   new_ship.plague_flag = old_ship.plague_flag;
   new_ship.ship_infected = old_ship.ship_infected;
   new_ship.ship_morale = old_ship.ship_morale;

   for (count = 0; count < 5; count++) {
      strcpy(new_ship.allies[count], old_ship.allies[count]);
   }

   new_ship.taxes = old_ship.taxes;
   new_ship.tax_warnings = old_ship.tax_warnings;
   new_ship.attack_sleds = old_ship.attack_sleds;

   for (count = 0; count < 15; count++) {
      new_ship.sled_xpos[count] = old_ship.sled_xpos[count];
      new_ship.sled_ypos[count] = old_ship.sled_ypos[count];
      new_ship.sled_universe[count] = old_ship.sled_universe[count];
      new_ship.sled_swarm[count] = old_ship.sled_swarm[count];
      new_ship.sled_power[count] = old_ship.sled_power[count];
   }

   strcpy(new_ship.last_at_by, old_ship.last_at_by);
   strcpy(new_ship.last_at_who, old_ship.last_at_who);

   new_ship.last_at_status = old_ship.last_at_status;
   new_ship.last_torp_count = old_ship.last_torp_count;
   new_ship.last_phaser_count = old_ship.last_phaser_count;
   new_ship.log_count = old_ship.log_count;

   strcpy(new_ship.base_death, old_ship.base_death);

   new_ship.base_hit_count = old_ship.base_hit_count;

   for (count = 0; count < 10; count++) {
      new_ship.x_planet_info[count] = old_ship.x_planet_info[count];
      new_ship.y_planet_info[count] = old_ship.y_planet_info[count];
   }

   new_ship.outstanding_bid = old_ship.outstanding_bid;
   new_ship.bid_amount = old_ship.bid_amount;

   strcpy(new_ship.base_boarded, old_ship.base_boarded);

   for (count = 0; count < 10; count++) {
      new_ship.planet_taxes[count] = old_ship.planet_taxes[count];
      new_ship.tax_xpos[count] = old_ship.tax_xpos[count];
      new_ship.tax_ypos[count] = old_ship.tax_ypos[count];
      strcpy(new_ship.slot_owned[count], old_ship.slot_owned[count]);
   }

   for (count = 0; count < OWNABLE; count++) {
      new_ship.owned_planets[count] = old_ship.owned_planets[count];
   }

   new_ship.allow_colors = old_ship.allow_colors;
   new_ship.sick_bay = old_ship.sick_bay;
   new_ship.bounty = old_ship.bounty;

   strcpy(new_ship.who_destroyed, old_ship.who_destroyed);

   for (count = 0; count < 10; count++) {
      new_ship.scout_xpos[count] = old_ship.scout_xpos[count];
      new_ship.scout_ypos[count] = old_ship.scout_ypos[count];
      new_ship.scout_to_x[count] = old_ship.scout_to_x[count];
      new_ship.scout_to_y[count] = old_ship.scout_to_y[count];
      new_ship.scout_direction[count] = old_ship.scout_direction[count];
      new_ship.scout_universe[count] = old_ship.scout_universe[count];
      new_ship.stay_on_station[count] = old_ship.stay_on_station[count];
   }

   strcpy(new_ship.pirate_code, old_ship.pirate_code);

   new_ship.leashed_by = old_ship.leashed_by;
   new_ship.leashed_to = old_ship.leashed_to;

   new_ship.local = old_ship.local;

/*
   New elements
*/

   for (count = 0; count < 20; count++) {
      new_ship.point_tag[count][0] = '-';
   }

   for (count = 0; count < MACROS; count++)
      new_ship.macro[count][0] = (char)NULL;
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

static int extract_config(void)
{
   char bail_out;
   char count;

   bail_out = FALSE;

   while (! bail_out) {
      if (! feof(configuration_file)) {
         fgets(record, 200, configuration_file);
	 if (! feof(configuration_file)) {
	    if (! feof(configuration_file) && record[0] != ';' && strlen(record) > 2) {
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

   if feof(configuration_file) {
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
   * Read out a config string up to the end of string or to the ;.      *
   * Then strip out the trailing spaces and tabs. Return the string.    *
   *                                                                    *
   ********************************************************************** */

static void cfg_string(void)
{
   char bail_out;
   char hold_record[201], *hold_point;
   char count;

   bail_out = FALSE;

   while (! bail_out) {
      if (! feof(configuration_file)) {
         fgets(hold_record, 200, configuration_file);
	 if (! feof(configuration_file)) {
	    hold_point = hold_record;
	    while (*hold_point && *hold_point == ' ') hold_point++;
	    strcpy(record, hold_point);
	    if (! feof(configuration_file) && record[0] != ';' && strlen(record) > 2) {
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

   if (feof(configuration_file)) {
      fputs("SHIP.CFG is corrupted!\n\r", stderr);
      exit(0);
   }

   for (count = 0; record[count]; count++) {
      if (record[count] == 0x3b) {
         record[count] = (char)NULL;
         break;
      }
   }

   while (record[strlen(record) - 1] == ' ' ||
      record[strlen(record) - 1] == 0x09) {
      record[strlen(record) - 1] = (char)NULL;
   }
}

/* **********************************************************************
   * The main entry point.						*
   *									*
   ********************************************************************** */

void main()
{
   char answer = (char)NULL;
   char in_buffer[201];
   short read_record = 0;

/*
   Make sure the SysOp knows what's going to happen.
*/

   clrscr();
   textcolor(LIGHTGREEN);
   printf("\nUniversal Mayhem\n\n");
   textcolor(WHITE);
   printf("This program will convert your older data files of Universal\n");
   printf("Mayhem, (Version 2.0 through 2.2), over to the new data file\n");
   printf("format of Version 2.3 and 2.4.\n\n");
   textcolor(LIGHTRED);
   printf("I will convert the following files and make back ups just in\n");
   printf("case something goes wrong along the way:\n\n");
   printf("SHIP.DAT     backed up to     SHIP.OLD\n");
   textcolor(WHITE);
   printf("All other data files remain unchanged.\n\n");
   textcolor(LIGHTRED);

   while (answer != 'y' && answer != 'n') {
      printf("Shall I proceed (yes or no)? ");
      fgets(in_buffer, 200, stdin);
      answer = tolower(in_buffer[0]);
   }

   textcolor(WHITE);

   if (answer == 'n') {
      textcolor(WHITE);
      exit(0);
   }

   clrscr();
   textcolor(WHITE);
   printf("If I have any problems with the conversion, I will let you\n");
   printf("know and restore the files back the way they were.\n\n");
   answer = (char)NULL;

   while (answer != 'y' && answer != 'n') {
      printf("I'm going to convert. Shall I start (yes or no)? ");
      fgets(in_buffer, 200, stdin);
      answer = tolower(in_buffer[0]);
   }

   textcolor(WHITE);

   if (answer == 'n') {
      exit(0);
   }

   clrscr();
   printf("Converting SHIP.DAT file...\n\n");

/*
   Make copy of the SHIP.DAT file and the goals file
*/

   system("copy ship.dat ship.old");

/*
   Get the players data file opened and create a temporary file.
*/

   if ((ship_in_file = mayhem_fopen("SHIP.DAT", "rb", ship_in_file)) == (FILE *)NULL) {
      clrscr();
      textcolor(LIGHTRED);
      printf("I was unable to open file: SHIP.DAT. Conversion did not\n");
      printf("take place.\n\n");
      exit(10);
   }

   if ((ship_out_file = mayhem_fopen("SHIP.NEW", "wb", ship_out_file)) == (FILE *)NULL) {
      clrscr();
      textcolor(LIGHTRED);
      printf("I was unable to create file: SHIP.NEW. Conversion did not\n");
      printf("take place.\n\n");
      exit(11);
   }

   if ((configuration_file = mayhem_fopen("SHIP.CFG", "r", configuration_file)) == (FILE *)NULL) {
      clrscr();
      textcolor(LIGHTRED);
      printf("I was unable to open file: SHIP.CFG. Conversion did not\n");
      printf("take place.\n\n");
      exit(13);
   }

   (void)extract_config();
   (void)extract_config();
   cfg_string();
   (void)extract_config();
   cfg_string();
   cfg_string();
   total_players = extract_config();
   mayhem_fclose(&configuration_file);

   printf("\nFiles have been opened and temporary files created...\n");
   printf("We have %d player records in SHIP.DAT to convert.", total_players);

/*
   Now that the initial stuff has been taken care of, we can start
   to perform the conversions.

   Enter into a loop which will read, convert, then write out new player
   data file information.
*/

   for (read_record = 0; read_record < total_players; read_record++) {
      read_old_ship_format(read_record);
      insert_new();
      write_new_ship_format(read_record);
   }

   mayhem_fclose(&ship_in_file);
   mayhem_fclose(&ship_out_file);


   system("del ship.dat");
   system("ren ship.new ship.dat");

   clrscr();
   printf("The new SHIP.DAT file has been created, (and a backup of the\n");
   printf("old one retained in SHIP.OLD)\n\n");
   printf("If you have any troubles, please let me know immediatly and\n");
   printf("be sure to make a copy of SHIP.OLD so that I\n");
   printf("may find the problem!\n\n");
   printf("Fredric Rice\n");
   printf("1:102/901 Mayhem West Coast 1-818-335-0128\n");
   exit(0);
}

/* **********************************************************************
   * Read a record from the old ships file.				*
   *									*
   ********************************************************************** */

static char read_old_ship_format(short record_number)
{
   UL dpoint;

   dpoint = ((long) record_number * sizeof(struct old_ship_file));

   if (fseek(ship_in_file, dpoint, 0) != 0) {
      printf("I was unable to point to a record in the old file SHIP.DAT\n");
      printf("It was on record number %d", record_number);
      exit(15);
   }

   if ((mayhem_fread(&old_ship, sizeof(struct old_ship_file), 1, ship_in_file)) != 1) {
      printf("I was unable to read a record in the old file: SHIP.DAT\n");
      printf("It was on record number %d\n\n", record_number);
      printf("Are there really %d players? SHIP.CFG says so", total_players);
      exit(15);
   }

   return(TRUE);
}

/* **********************************************************************
   * Take the data and store it to the new file.			*
   *									*
   ********************************************************************** */

static char write_new_ship_format(short record_number)
{
   UL dpoint;

   dpoint = ((long) record_number * sizeof(struct new_ship_file));

   if (fseek(ship_out_file, dpoint, 0) != 0) {
      printf("I was unable to point to a record in the new file SHIP.NEW\n");
      printf("It was on record number %d", record_number);
      exit(15);
   }

   if ((mayhem_fwrite(&new_ship, sizeof(struct new_ship_file), 1, ship_out_file)) != 1) {
      printf("I was unable to write a record in the new file: SHIP.NEW\n");
      printf("It was on record number %d", record_number);
      exit(15);
   }

   return(TRUE);
}

/* **********************************************************************
   * Check to see if the file is already open. If so, report a log and	*
   * return the existing pointer.					*
   *									*
   ********************************************************************** */

FILE *mayhem_fopen(char *fname, char *types, FILE *fpoint)
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

int mayhem_fclose(FILE **fname)
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

int mayhem_fcloseall(void)
{
   if (mayhem_fclose(&ship_in_file) == 0) {
      ship_in_file = (FILE *)NULL;
   }

   if (mayhem_fclose(&ship_out_file) == 0) {
      ship_out_file = (FILE *)NULL;
   }

   if (mayhem_fclose(&configuration_file) == 0) {
      configuration_file = (FILE *)NULL;
   }

   return(0);
}

/* **********************************************************************
   * Make a read. Before this is done, however, make sure that the file	*
   * has been opened. If not, report a failure and return 0 bytes read.	*
   *									*
   ********************************************************************** */

int mayhem_fread(void *inbuf, size_t tsize, size_t tcount, FILE *fname)
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

int mayhem_fwrite(void *outbuf, size_t tsize, size_t tcount, FILE *fname)
{
   if (fname == (FILE *)NULL) {
      return(0);
   }

   return(fwrite(outbuf, tsize, tcount, fname));
}

