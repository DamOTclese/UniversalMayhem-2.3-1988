
/* **********************************************************************
   * This stand-alone program will allow the editing of the Universal   *
   * Mayhem users data file SHIP.DAT.                                   *
   *                                                                    *
   * Copyright by Fredric L. Rice, 1990, 1991.                          *
   * All rights reserved.                                               *
   *                                                                    *
   *           outportb(0x64, 0xfe);  Keyboard reset                    *
   *                                                                    *
   ********************************************************************** */

#include "defines.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "conio.h"

/* **********************************************************************
   * This is the structure of the SHIP.DAT data file.			*
   *									*
   * As you can see, it contains all of the information needed for the	*
   * retention of all things the player has control/ownership over.	*
   *									*
   ********************************************************************** */

struct ship_record {          /* Structure of SHIP.DAT file                 */
  long ship_xpos;           /* The record number (xpos) of where ship is  */
  long ship_ypos;           /* The ypos of where the ship is              */
  UC ship_universe;	    /* The universe the ship is in.		  */
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
  UC base_universe;	    /* The universe the base is in		  */
  UL base_credits;          /* The amount of money in the supply base     */
  UL base_cargo;            /* The amount of cargo in the supply base     */
  UL base_crew;             /* The number of crew members in base         */
  short base_cloak;         /* The class cloak of the base                */
  long base_shield;         /* The number of energy units in base shield  */
  char ship_person[31];     /* The personal name of the captain of ship   */
  short rem_xpos[10];       /* Record numbers (yposes) of remotes         */
  short rem_ypos[10];       /* xposes of four remote robot sensors        */
  UC rem_universe[10];	    /* The universe the remote is in		  */
  short total_kills;        /* The number of times this ship has killed   */
  short time_remaining;     /* Number of minutes remaining to play        */
  short planets_owned;      /* Number of planets captain has named        */
  short plague_flag;        /* 0 is no plague, else greater than 0.       */
  short ship_infected;      /* Is ship infected? Holds class infection    */
  short ship_morale;        /* Morale factor from 0% to 100%	     	  */
  char allies[5][5];        /* Up to 5 ships to be allied with names      */
  float taxes;	            /* Accumulative taxes for Galactic Police.    */
  char tax_warnings;        /* Number of warnings on taxes / Auction flg  */
  short attack_sleds;	    /* Number of attack sleds on board	     	  */
  short sled_xpos[15];	    /* Xposition of up to 15 attack sleds	  */
  short sled_ypos[15];	    /* Yposition of up to 15 attack sleds	  */
  UC sled_universe[15];	    /* The universe the sled is in		  */
  short sled_swarm[15];     /* Number of ships in up to 15 attack swarms  */
  short sled_power[15];	    /* Average power available for the swarms     */
  char last_at_by[5];	    /* Name of ship that last attacked this ship  */
  char last_at_who[5];	    /* Name of ship this ship last attacked	  */
  char last_at_status;	    /* Status of the last ship attacked 	  */
  short last_torp_count;    /* number of torps by last attacked by	  */
  float last_phaser_count;  /* Number of millions of units last atkd by   */
  short log_count;	    /* Number of times captain has signed in      */
  char base_death[5];	    /* Who destroyed this ships base?	          */
  short base_hit_count;	    /* Number of times base hit while unshielded  */
  long x_planet_info[10];   /* X-Position of up to ten planets or points  */
  long y_planet_info[10];   /* Y-Position of up to ten planets or points  */
  char outstanding_bid;     /* Ships number bidded on		     	  */
  long bid_amount;	    /* Amount of the bid			  */
  char base_boarded[5];     /* Allied ship that last boarded ships base   */
  float planet_taxes[10];   /* Taxes owed at ten planets.		  */
  long tax_xpos[10];	    /* Xpositions of the ten tax due planets      */
  long tax_ypos[10];	    /* Ypositions of the ten tax due planets	  */
  char slot_owned[10][5];   /* Name of the ship that owns taxed planet	  */
  short owned_planets[100]; /* Xpos of 1 of 4 possible ypos of owned      */
  unsigned allow_colors : 1;/* Set to either 'Y' or 'N' for ansi colors   */
  short sick_bay;	    /* Number of crew members in sick bay.	  */
  long bounty;		    /* Number of credits available for bounty	  */
  char who_destroyed[5];    /* Name of ship that destroyed this ship.     */
  short scout_xpos[10];	    /* Xpos of the scouts			  */
  short scout_ypos[10];	    /* Ypos of the scouts			  */
  short scout_to_x[10];     /* Xpos of where to warp to			  */
  short scout_to_y[10];	    /* Ypos of where to warp to			  */
  char scout_direction[10]; /* 0-Outbound, 1-Station keeping , 2-Inbound  */
  UC scout_universe[10];    /* What universe is the scout in?		  */
  char stay_on_station[10]; /* Will it stay on station? No? It'll return. */
  char pirate_code[30];     /* The pirate access code                     */
  char leashed_by;	    /* Ship this one is leased by		  */
  char leashed_to;	    /* Ship this one has got a leash on		  */
  char local;               /* 0 if local, 1 if imported                  */
  char point_tag[20][21];   /* Symbolic name of x/y posistion tag name.   */
  char macro[MACROS][201];  /* Keyboard Macros.                           */
} ;

   static struct ship_record ship;

/* **********************************************************************
   * Data variables needed.                                             *
   *                                                                    *
   ********************************************************************** */

   static unsigned short current_record;
   static char record[201];
   static unsigned short total_players;
   static UL xsize, ysize;
   static char the_date[27];
   static short time_remaining;
   static FILE *ship_file;
   static FILE *config;
   static char modified[250];

/* **********************************************************************
   * Make a routine which will simply zero out the structures elements  *
   * or set them to various start-up values. This is done as a funcion  *
   * because there are more than 1 function which requires it.          *
   *                                                                    *
   ********************************************************************** */

void make_zero_record(void)
{
   unsigned short counta;

   ship.ship_xpos = xsize / 2;
   ship.ship_ypos = ysize / 2;
   ship.ship_universe = 0;
   ship.ship_power = 50000L;
   ship.ship_shield = 10000;
   ship.ship_credits = 10000;
   ship.ship_cargo = 300;
   ship.ship_crew = 500;
   ship.ship_shuttle = 1;
   ship.ship_hull = 100;
   ship.ship_cloak = 10;
   ship.ship_sensor = 10;
   STRNCPY(ship.ship_name, "NONE", 4);
   STRNCPY(ship.ship_pass, "NONE", 4);
   STRNCPY(ship.ship_date, the_date, 26);
   ship.ship_warp = 10;
   ship.ship_torp = 200;
   ship.base_xpos = 0;
   ship.base_ypos = 0;
   ship.base_universe = 0;
   ship.base_credits = 0;
   ship.base_cargo = 0;
   ship.base_crew = 0;
   ship.base_cloak = 0;
   ship.base_shield = 0;
   strcpy(ship.ship_person, "No name");

   for (counta = 0; counta < 10; counta++) {
      ship.rem_xpos[counta] = 0L;
      ship.rem_ypos[counta] = 0L;
      ship.rem_universe[counta] = 0;
   }

   ship.total_kills = 0;
   ship.time_remaining = time_remaining;
   ship.planets_owned = 0;
   ship.plague_flag = 0;
   ship.ship_infected = 0;
   ship.ship_morale = 100;
   ship.taxes = 0.0;
   ship.attack_sleds = 0;

   for (counta = 0; counta < 5; counta++) {
      strcpy(ship.allies[counta], "NONE");
   }

   for (counta = 0; counta < 15; counta++) {
      ship.sled_xpos[counta] = (short)NIL;
      ship.sled_ypos[counta] = (short)NIL;
      ship.sled_swarm[counta] = (short)NIL;
      ship.sled_power[counta] = 0;
      ship.sled_universe[counta] = 0;
   }

   strcpy(ship.last_at_by, "NONE");
   strcpy(ship.last_at_who, "NONE");
   ship.last_at_status = 0;
   ship.log_count = 0;
   strcpy(ship.base_death, "NONE");
   ship.base_hit_count = 0;

   for (counta = 0; counta < 10; counta++) {
      ship.x_planet_info[counta] = 0;
      ship.y_planet_info[counta] = 0;
      ship.point_tag[counta][0] = '-';
   }

   ship.outstanding_bid = (char)NIL;
   strcpy(ship.base_boarded, "NONE");

   for (counta = 0; counta < 10; counta++) {
      ship.planet_taxes[counta] = 0.0;
      ship.tax_xpos[counta] = (short)NIL;
      ship.tax_ypos[counta] = (short)NIL;
      strcpy(ship.slot_owned[counta], "NONE");
   }

   for (counta = 0; counta < 100; counta++) {
      ship.owned_planets[counta] = (short)NIL;
   }

   ship.sick_bay = 0;
   ship.bounty = 0L;
   strcpy(ship.who_destroyed, "NONE");

   for (counta = 0; counta < 10; counta++) {
      ship.scout_xpos[counta] = (short)NIL;
      ship.scout_ypos[counta] = (short)NIL;
      ship.scout_to_x[counta] = (short)NIL;
      ship.scout_to_y[counta] = (short)NIL;
      ship.scout_direction[counta] = (char)NIL;
      ship.scout_universe[counta] = 0;
      ship.stay_on_station[counta] = TRUE;
   }

   strcpy(ship.pirate_code, "NONE");
   ship.leashed_by = (char)NIL;
   ship.leashed_to = (char)NIL;
   ship.local = 0;

   for (counta = 0; counta < MACROS; counta++)
      ship.macro[counta][0] = (char)NULL;
}

/* **********************************************************************
   * Read in the ship record.                                           *
   *                                                                    *
   ********************************************************************** */

void read_record(unsigned int the_record)
{
   unsigned long dpoint;
        
   dpoint = ((long)the_record * sizeof(struct ship_record));

   if (fseek(ship_file, dpoint, 0) != 0) {
      printf("\nUnable to point to record in the ship file.");
      printf("\nTried to point to: %ld, record %d\n", dpoint, the_record);
      exit(0);
    }

   if ((fread(&ship, sizeof(struct ship_record), 1, ship_file)) != 1) {
      printf("\nError occured when reading ships record.\n");
      printf("Tried to point to: %ld, record %d", dpoint, the_record);
      exit(0);
    }
}

/* **********************************************************************
   * Write out the record.                                              *
   *                                                                    *
   ********************************************************************** */

void write_record(unsigned int the_record)
{
   unsigned long dpoint;
   dpoint = ((long) the_record * sizeof(struct ship_record));

   if (fseek(ship_file, dpoint, 0) != 0) {
      printf("\nUnable to point to record in the ship file.\n");
      printf("Tried to point to: %ld, record %d\n", dpoint, the_record);
      exit(0);
   }

   if ((fwrite(&ship, sizeof(struct ship_record), 1, ship_file)) != 1) {
      printf("\nError occured when write of ship record.\n");
      printf("Point to %ld, Record: %d", dpoint, the_record);
      exit(0);
   }
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

   if feof(config) {
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
      if (! feof(config)) {
         fgets(hold_record, 200, config);
         if (! feof(config)) {
            hold_point = hold_record;
            while (*hold_point && *hold_point == ' ') hold_point++;
            strcpy(record, hold_point);
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
   * This routine will take string array dbuffer and convert all of the *
   * characters within it inbto uppercase that are valid in uppercase.  *
   * This is done so that END may be checked for. Some compilers supply *
   * a method for converting entire strings to upper case so this may   *
   * not be required by other systems. "toupper()" didn't work here so  *
   * we subtract.                                                       *
   *                                                                    *
   ********************************************************************** */

static void ucase(char *this_record)
{
   while (*this_record) {
      if (*this_record > 0x60 && *this_record < 0x7b) {
         *this_record = *this_record - 32;
      }
      this_record++;
   }
}

/* **********************************************************************
   * If the file is corrupted, see if it should be fixed.               *
   *                                                                    *
   ********************************************************************** */

static void rebuild_record(unsigned short count)
{
    printf("\n---> Entry %d is CORRUPTED! <---\n\n", count);
    printf("Should I correct this entry? (Y)es: ");
    fgets(record, 200, stdin);
    ucase(record);
    if (record[0] != 'Y' || strlen(record) == 1) return;
    make_zero_record();
    write_record(count);
}

/* **********************************************************************
   * See if the players record is valid.                                *
   *                                                                    *
   ********************************************************************** */

static void is_acceptable(short count)
{
    char ctest;

    if (count == 0) return;

    if (strlen(ship.ship_name) != 4) {
        rebuild_record(count);
        return;
    }

    for (ctest = 0; ctest < 4; ctest++) {
        if (ship.ship_name[ctest] < 'A' || ship.ship_name[ctest] > 'Z') {
            if (ship.ship_name[ctest] < '0' || ship.ship_name[ctest] > '9') {
               rebuild_record(count);
               return;
            }
        }
    }
}

/* **********************************************************************
   * Validate all players.                                              *
   *                                                                    *
   ********************************************************************** */

void validate_ship_file(void)
{
    unsigned short count;

    for (count = 0; count < total_players; count++) {
	read_record(count);

        if (strcmp(ship.ship_name, "NONE")) {
            is_acceptable(count);
        }
    }
}
/* **********************************************************************
   *                                                                    *
   ********************************************************************** */

static void display_all_ships(void)
{
    short count;
    char x, y;
    y = 2;
    x = 1;

    for (count = 0; count < total_players; count++) {
        read_record(count);
	if (strcmp(ship.ship_name, "NONE")) {
	    gotoxy(x, y);
            printf("%c%03d - %4s", modified[count] ? '*' : ' ', count, ship.ship_name);
	    y++;
	    if (y == 20) {
                y = 2;
                x += 12;
            }
        }
    }
}

/* **********************************************************************
   * Allow editing of the remotes.                                      *
   *                                                                    *
   * In all versions, there have been up to ten of them.                *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_remotes(void)
{
    char count, xpos, ypos, universe;

ask_again:
    clrscr();

    for (count = 0; count < 10; count++) {
        if (ship.rem_xpos[count] == 0 && ship.rem_ypos[count] == 0) {
            printf("Remote %02d is not bought\n", count + 1);
        }
        else {
            printf("Remote %02d is at {%04d-%04d} in universe %d\n",
                count + 1,
                ship.rem_xpos[count],
                ship.rem_ypos[count],
                ship.rem_universe[count]);
        }
    }

    printf("\nEnter remote number (1 to 10) or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return; /* Carriage return counts as 1 */
    count = atoi(record);
    if (count < 1 || count > 10) goto ask_again;
    count--;

ask_x_pos:
    printf("Enter new X posistion: (%d): ", ship.rem_xpos[count]);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        xpos = atoi(record);
    else
        xpos = ship.rem_xpos[count];

    if (xpos < 0 || xpos > xsize - 1) goto ask_x_pos;

ask_y_pos:
    printf("Enter new Y posistion: (%d): ", ship.rem_ypos[count]);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        ypos = atoi(record);
    else
        ypos = ship.rem_ypos[count];

    if (ypos < 0 || ypos > ysize - 1) goto ask_y_pos;

ask_universe:
    printf("Enter new universe: (from 0 to 12) (Default %d): ", ship.rem_universe[count]);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        universe = atoi(record);
    else
        universe = ship.rem_universe[count];

    if (universe < 0 || universe > 12) goto ask_universe;

    ship.rem_xpos[count] = xpos;
    ship.rem_ypos[count] = ypos;
    ship.rem_universe[count] = universe;
    goto ask_again;
}

/* **********************************************************************
   * Allow the editing of the alliance elements.                        *
   *                                                                    *
   * In all versions, there have been up to five of them.               *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_alliance(void)
{
    char count;

ask_again:
    clrscr();

    for (count = 0; count < 5; count++)
        printf("Alliance %d to %s\n", count + 1, ship.allies[count]);

    printf("\nEnter item number (from 1 to 5) to edit or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return; /* Carriage return counts as 1 */
    count = atoi(record);
    if (count < 1 || count > 5) goto ask_again;
    count--;
    printf("Enter new name for alliance (Hit [ENTER] to erase): ");
    fgets(record, 200, stdin);

    if (strlen(record) == 1) {
        strcpy(ship.allies[count], "NONE");
    }
    else if (strlen(record) != 5) {
        printf("Must be a 4 letter ships name!\n");
    }
    else {
        ucase(record);
        STRNCPY(ship.allies[count], record, 4);
    }

    goto ask_again;
}

/* **********************************************************************
   * Allow the editing of the attack swarms.                            *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_swarm(void)
{
    char count;
    short xpos, ypos, swarm, power, attack_sleds;
    UC universe;

ask_again:
    clrscr();

    for (count = 0; count < 15; count++) {
        if (ship.sled_xpos[count] != (short)NIL) {
            printf("Swarm %02d, {%d-%d} in universe %02d, %d total, %d power\n",
                count + 1,
                ship.sled_xpos[count],
                ship.sled_ypos[count],
                ship.sled_universe[count],
                ship.sled_swarm[count],
                ship.sled_power[count]);
        }
        else {
            printf("Swarm %02d, Not deployed\n", count + 1);
        }
    }

    printf("\nAttack sleds on board: %d\n\n", ship.attack_sleds);

    printf("Enter swarm number, (O)nboard, or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return;
    ucase(record);

    if (record[0] == 'O') {
        printf("\nEnter new value for total attack sleds on board ship: (%d): ", ship.attack_sleds);
        fgets(record, 200, stdin);
        if (strlen(record) != 1)
            ship.attack_sleds = atoi(record);
        goto ask_again;
    }

    count = atoi(record);
    if (count < 1 || count > 15) goto ask_again;
    count--;

    xpos = ship.sled_xpos[count];
    ypos = ship.sled_ypos[count];
    universe = ship.sled_universe[count];
    swarm = ship.sled_swarm[count];
    power = ship.sled_power[count];

ask_x_pos:
    printf("Enter new X posistion (Enter 0 to recall sleds): (%d): ", xpos);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        xpos = atoi(record);

    if (xpos == 0) {
        xpos = ypos = (short)NIL;
        universe = power = 0;
        goto recall_sled;
    }

    if (xpos < 0 || xpos > xsize - 1) goto ask_x_pos;

ask_y_pos:
    printf("Enter new Y posistion (Enter 0 to recall sleds): (%d): ", ypos);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        ypos = atoi(record);

    if (ypos == 0) {
        xpos = ypos = (short)NIL;
        universe = power = 0;
        goto recall_sled;
    }

    if (ypos < 0 || ypos > ysize - 1) goto ask_y_pos;

ask_universe:
    printf("Enter new universe: (from 0 to 12) (Default %d): ", universe);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        universe = atoi(record);

    if (universe < 0 || universe > 12) goto ask_universe;

    printf("Enter new total for swarm: (%d): ", swarm);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        swarm = atoi(record);

    printf("Enter new power level for swarm: (%d): ", power);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        power = atoi(record);

recall_sled:
    ship.sled_xpos[count] = xpos;
    ship.sled_ypos[count] = ypos;
    ship.sled_universe[count] = universe;
    ship.sled_swarm[count] = swarm;
    ship.sled_power[count] = power;
    goto ask_again;
}

/* **********************************************************************
   * Who last did what to who?                                          *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_last(void)
{
    char count;

ask_again:
    clrscr();

    printf("1) Last attacked by %s\n", ship.last_at_by);
    printf("2) Last attacked ship %s\n", ship.last_at_who);
    printf("   Damage to enemy ship result: \n");

    switch (ship.last_at_status) {
        case 0: printf("     A) You did no damage!\n");
                break;
        case 1: printf("     B) You damaged it!\n");
                break;
        case 2: printf("     C) You dropped its shields!\n");
                break;
        case 3: printf("     D) You left it drifting!\n");
                break;
        case 4: printf("     E) You destroyed it!\n");
                break;
        case 5: printf("     F) You boarded it and left it drifting!\n");
                break;
    }

    printf("3) Last hit by %d torps\n", ship.last_torp_count);
    printf("4) Last hit by %ld phaser units\n", ship.last_phaser_count);
    printf("\nEnter item number, letter, or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return;
    ucase(record);

    switch(record[0]) {
        case 'A': ship.last_at_status = 0; goto ask_again;
        case 'B': ship.last_at_status = 1; goto ask_again;
        case 'C': ship.last_at_status = 2; goto ask_again;
        case 'D': ship.last_at_status = 3; goto ask_again;
        case 'E': ship.last_at_status = 4; goto ask_again;
        case 'F': ship.last_at_status = 5; goto ask_again;
    }

    count = atoi(record);
    if (count < 0 || count > 4) goto ask_again;

    switch(count) {
ask_1:
        case 1: printf("Enter new last attacked by [ENTER] for none: ");
                fgets(record, 200, stdin);
                if (strlen(record) != 1) {
                    if (strlen(record) != 5) {
                        printf("Name needs to be 4 characters long!\n");
                        goto ask_1;
                    }
                    else {
                        STRNCPY(ship.last_at_by, record, 4);
                    }
                }
                else {
                    strcpy(ship.last_at_by, "NONE");
                }
                break;
        case 2: printf("Enter new last attacked ship [ENTER] for none: ");
ask_2:
                fgets(record, 200, stdin);
                if (strlen(record) != 1) {
                    if (strlen(record) != 5) {
                        printf("Name needs to be 4 characters long!\n");
                        goto ask_2;
                    }
                    else {
                        STRNCPY(ship.last_at_who, record, 4);
                    }
                }
                else {
                    strcpy(ship.last_at_who, "NONE");
                }
                break;
        case 3: printf("Enter last hit torp count: (%d): ", ship.last_torp_count);
                fgets(record, 200, stdin);
                if (strlen(record) != 1)
                    ship.last_torp_count = atoi(record);
                break;
        case 4: printf("Enter hit by phaser value: (%ld): ", ship.last_phaser_count);
                fgets(record, 200, stdin);
                if (strlen(record) != 1)
                    ship.last_phaser_count = atoi(record);
                break;
    }

    goto ask_again;
}

/* **********************************************************************
   * Allow the NAV planets to be edited.                                *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_nav(void)
{
    char count;

ask_again:
    clrscr();

    for (count = 0; count < 10; count++) {
        if (ship.x_planet_info[count] == 0l && ship.y_planet_info[count] == 0l) {
            printf("%02d) Empty NAV data bank\n", count + 1);
        }
        else {
            printf("%02d) %s at {%ld-%ld} %s\n",
                count + 1,
                ship.point_tag[count][0] == '-' ? "Planet" : "Point-In-Space",
                ship.x_planet_info[count],
                ship.y_planet_info[count],
                ship.point_tag[count][0] == '-' ? "" : ship.point_tag[count]);
        }
    }

    printf("\nEnter item number to edit or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return;
    count = atoi(record);
    if (count < 1 || count > 10) goto ask_again;
    count--;

ask_x_again:
    printf("Enter new X posistion: (%ld): ", ship.x_planet_info[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.x_planet_info[count] = atoi(record);
        if (ship.x_planet_info[count] < 0 || ship.x_planet_info[count] > xsize - 1) {
            goto ask_x_again;
        }
    }

ask_y_again:
    printf("Enter new Y posistion: (%ld): ", ship.y_planet_info[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.y_planet_info[count] = atoi(record);
        if (ship.y_planet_info[count] < 0 || ship.y_planet_info[count] > ysize - 1) {
            goto ask_y_again;
        }
    }

ask_planet:
    printf("Should this be a (P)lanet or a point-in-(S)pace: ");
    fgets(record, 200, stdin);
    ucase(record);
    if (record[0] == 'P') {
        ship.point_tag[count][0] = '-';
    }
    else if (record[0] == 'S') {
        printf("Enter symbolic name: (%s): ", ship.point_tag[count]);
        fgets(record, 200, stdin);
        if (strlen(record) != 1) {
            STRNCPY(ship.point_tag[count], record, 20);
        }
    }
    else {
        goto ask_planet;
    }

    goto ask_again;
}

/* **********************************************************************
   * Allow the setting of the taxes owed at the various planets.        *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_tax_planet(void)
{
    char count;

ask_again:
    clrscr();

    for (count = 0; count < 10; count++) {
        printf("%02d) Owed %ld credits at {%ld-%ld}, ship name %s\n",
            count + 1,
            ship.planet_taxes[count],
            ship.tax_xpos[count],
            ship.tax_ypos[count],
            ship.slot_owned[count]);
    }

    printf("\nEnter item number or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return;
    count = atoi(record);
    if (count < 1 || count > 10) goto ask_again;
    count--;

    printf("Enter new taxes owed (Enter 0 for none): (%ld): ", ship.planet_taxes[count]);
    fgets(record, 200, stdin);

    if (strlen(record) != 1)
        ship.planet_taxes[count] = atol(record);

    if (ship.planet_taxes[count] == 0) {
        ship.tax_xpos[count] = ship.tax_ypos[count] = (short)NIL;
        strcpy(ship.slot_owned[count], "NONE");
        return;
    }

ask_x_again:
    printf("Enter new X posistion for planet: (%ld): ", ship.tax_xpos[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.tax_xpos[count] = atoi(record);
        if (ship.tax_xpos[count] < 0 || ship.tax_xpos[count] > xsize - 1) {
            goto ask_x_again;
        }
    }

ask_y_again:
    printf("Enter new Y posistion for planet: (%ld): ", ship.tax_ypos[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.tax_ypos[count] = atoi(record);
        if (ship.tax_ypos[count] < 0 || ship.tax_ypos[count] > ysize - 1) {
            goto ask_y_again;
        }
    }

ask_name_again:
    printf("Enter name of owner: [ENTER] to set to NONE: ");
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        if (strlen(record) != 5) {
            printf("Ship name must be 4 characters!\n");
            goto ask_name_again;
        }
        STRNCPY(ship.slot_owned[count], record, 4);
    }
    else {
        strcpy(ship.slot_owned[count], "NONE");
    }

    goto ask_again;
}

/* **********************************************************************
   * Scout ship maintenance.                                            *
   *                                                                    *
   *  char scout_direction[10]:                                         *
   *     0-Outbound, 1-Station keeping, 2-Inbound                       *
   *                                                                    *
   ********************************************************************** */

static void sub_menu_scout(void)
{
    char count;
    char direction[20];

ask_again:
    clrscr();

    for (count = 0; count < 10; count++) {
        if (ship.scout_xpos[count] != (short)NIL) {
            switch (ship.scout_direction[count]) {
                case 0: strcpy(direction, "Outbound"); break;
                case 1: strcpy(direction, "Station-keeping"); break;
                case 2: strcpy(direction, "Inbound"); break;
            }
            printf("%02d) Scout at {%d-%d} universe %d to {%d-%d}, %s, will %s\n",
                count + 1,
                ship.scout_xpos[count],
                ship.scout_ypos[count],
                ship.scout_universe[count],
                ship.scout_to_x[count],
                ship.scout_to_y[count],
                ship.stay_on_station[count] ? "'Stick'" : "Return");
        }
        else {
            printf("%02d) Scout not active\n", count + 1);
        }
    }

    printf("\nEnter item number to edit or [ENTER] to keep: ");
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return;
    count = atoi(record);
    if (count < 1 || count > 10) goto ask_again;
    count--;

ask_x_again:
    printf("Enter new X posistion (Enter 0 to recall): (%d): ", ship.scout_xpos[count]);
    fgets(record, 200, stdin);

    if (strlen(record) != 1) {
        if (atoi(record) == 0) {
            ship.scout_xpos[count] = (short)NIL;
            ship.scout_ypos[count] = (short)NIL;
            ship.scout_to_x[count] = (short)NIL;
            ship.scout_to_y[count] = (short)NIL;
            ship.scout_direction[count] = (char)NIL;
            ship.scout_universe[count] = 0;
            ship.stay_on_station[count] = TRUE;
            goto ask_again;
        }

        ship.scout_xpos[count] = atoi(record);

        if (ship.scout_xpos[count] < 1 || ship.scout_xpos[count] > xsize - 1) {
            goto ask_x_again;
        }
    }

ask_y_again:
    printf("Enter new Y posistion: (%d): ", ship.scout_ypos[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.scout_ypos[count] = atoi(record);
        if (ship.scout_ypos[count] < 1 || ship.scout_ypos[count] > ysize - 1) {
            goto ask_y_again;
        }
    }

ask_universe:
    printf("Enter universe: (0 to 12, Default %d): ", ship.scout_universe[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.scout_universe[count] = atoi(record);
        if (ship.scout_universe[count] < 0 || ship.scout_universe[count] > 12) {
            goto ask_universe;
        }
    }

ask_to_x_again:
    printf("Enter new Go-To X posistion: (%d): ", ship.scout_to_x[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.scout_to_x[count] = atoi(record);
        if (ship.scout_to_x[count] < 1 || ship.scout_to_x[count] > xsize - 1) {
            goto ask_to_x_again;
        }
    }

ask_to_y_again:
    printf("Enter new Go-To Y posistion: (%d): ", ship.scout_to_y[count]);
    fgets(record, 200, stdin);
    if (strlen(record) != 1) {
        ship.scout_to_y[count] = atoi(record);
        if (ship.scout_to_y[count] < 1 || ship.scout_to_y[count] > ysize - 1) {
            goto ask_to_y_again;
        }
    }

ask_stay:
    printf("Should scout ship %s? ", ship.stay_on_station[count] ? "'Stick' once its on station" : "Return to ship");
    fgets(record, 200, stdin);
    ucase(record);
    if (record[0] == 'N') {
        if (ship.stay_on_station[count]) {
            ship.stay_on_station[count] = FALSE;
        }
        else {
            ship.stay_on_station[count] = TRUE;
        }
    }
    else if (record[0] != 'Y') {
        goto ask_stay;
    }

ask_direction:
    printf("(O)utbound, (S)tation-Keeping, (I)nbound: (Pick one): ");
    fgets(record, 200, stdin);
    ucase(record);
    if (record[0] == 'O') {
        ship.scout_direction[count] = 0;
    }
    else if (record[0] == 'S') {
        ship.scout_direction[count] = 1;
    }
    else if (record[0] == 'I') {
        ship.scout_direction[count] = 2;
    }
    else {
        goto ask_direction;
    }

    goto ask_again;
}

/* **********************************************************************
   * Allow the edit of the specified records element.                   *
   *                                                                    *
   ********************************************************************** */

static void edit_element(char this_element)
{
    UL long_item;
    short int_item;
        
    modified[current_record] = TRUE;

    clrscr();
    printf("\n\n");

    switch(this_element) {
        case  1: printf("Ships name: %s\n", ship.ship_name);
case_1_again:
                 printf("\nEnter new ship name [4 characters], NONE to erase: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     ucase(record);
                     if (strlen(record) != 5) {
                         printf("New name is not valid!\n");
                         goto case_1_again;
                     }
                     STRNCPY(ship.ship_name, record, 4);
                 }
                 break;

        case  2: printf("Captains name: %s\n", ship.ship_person);
                 printf("\nEnter new captains name: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     STRNCPY(ship.ship_person, record, 30);
                     ship.ship_person[strlen(ship.ship_person) - 1] = (char)NULL;
                 }
                 break;

        case  3: printf("Ships location {X = %ld Y = %ld} Universe %d\n", ship.ship_xpos, ship.ship_ypos, ship.ship_universe);
                 if (ship.ship_xpos == xsize / 2 && ship.ship_ypos == ysize /  2) {
                     printf("This ship has not moved yet.\n");
                 }
case_3_1:
                 printf("\nEnter new X posistion: (%d): ", ship.ship_xpos);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_xpos;

                 if (long_item < 0 || long_item >= xsize) {
                     printf("Item invalid!\n");
                     goto case_3_1;
                 }
                 ship.ship_xpos = long_item;
case_3_2:
                 printf("\nEnter new Y posistion: (%d): ", ship.ship_ypos);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_ypos;

                 if (long_item < 0 || long_item >= ysize) {
                     printf("Item invalid!\n");
                     goto case_3_2;
                 }
                 ship.ship_ypos = long_item;
case_3_3:
                 printf("\nEnter new universe posistion: (%d): ", ship.ship_universe);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_universe;

                 if (long_item < 0 || long_item > 12) {
                     printf("Item invalid!\n");
                     goto case_3_3;
                 }
                 ship.ship_universe = (char)long_item;
                 break;

        case  4: printf("Base location {X = %ld Y = %ld} Universe %d\n", ship.base_xpos, ship.base_ypos, ship.base_universe);
case_4_1:
                 printf("\nEnter new X posistion: (%d): ", ship.base_xpos);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_xpos;

                 if (long_item < 0 || long_item > xsize - 1) {
                     printf("Item invalid!\n");
                     goto case_4_1;
                 }
                 ship.base_xpos = long_item;
case_4_2:
                 printf("\nEnter new Y posistion: (%d): ", ship.base_ypos);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_ypos;

                 if (long_item < 0 || long_item > ysize - 1) {
                     printf("Item invalid!\n");
                     goto case_4_2;
                 }
                 ship.base_ypos = long_item;
case_4_3:
                 printf("\nEnter new universe posistion: (%d): ", ship.base_universe);
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_universe;

                 if (long_item < 0 || long_item > 12) {
                     printf("Item invalid!\n");
                     goto case_4_3;
                 }
                 ship.base_universe = (char)long_item;
                 break;

        case  5: printf("Ship power %ld\n", ship.ship_power);
                 printf("\nEnter new ships power: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_power;

                 ship.ship_power = long_item;
                 break;

        case  6: printf("Ship shield %ld\n", ship.ship_shield);
                 printf("\nEnter new ships shield: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_shield;

                 ship.ship_shield = long_item;
                 break;

        case  7: printf("Ship credits %ld\n", ship.ship_credits);
                 printf("\nEnter new ships credits: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_credits;

                 ship.ship_credits = long_item;
                 break;

        case  8: printf("Ship cargo %d\n", ship.ship_cargo);
                 printf("\nEnter new ships cargo: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_cargo;

                 ship.ship_cargo = int_item;
                 break;

        case  9: printf("Ship crew %d\n", ship.ship_crew);
                 printf("\nEnter new ships crew: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_crew;

                 ship.ship_crew = int_item;
                 break;

        case 10: printf("Ship shuttle class %d\n", ship.ship_shuttle);
                 printf("\nEnter new shuttle class: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_shuttle;

                 ship.ship_shuttle = int_item;
                 break;

        case 11: printf("Ship hull level %d\n", ship.ship_hull);
case_11_again:
                 printf("\nEnter new hull level: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_hull;

                 if (int_item > 1000) {
                     printf("Maximum hull is 1000!\n");
                     goto case_11_again;
                 }
                 ship.ship_hull = int_item;
                 break;

        case 12: printf("Ship cloak class %d\n", ship.ship_cloak);
case_12_again:
                 printf("\nEnter new cloak class: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_cloak;

                 if (int_item > 200) {
                     printf("Maximum cloak is 200!\n");
                     goto case_12_again;
                 }
                 ship.ship_cloak = int_item;
                 break;

        case 13: printf("Ship sensor class %d\n", ship.ship_sensor);
case_13_again:
                 printf("\nEnter new sensor class: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_sensor;

                 if (int_item > 200) {
                     printf("Maximum sensor is 200!\n");
                     goto case_13_again;
                 }
                 ship.ship_sensor = int_item;
                 break;

        case 14: printf("Ship password %s\n", ship.ship_pass);
                 printf("\nEnter new ships password: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1) {
                     ucase(record);
                     STRNCPY(ship.ship_pass, record, 20);
                     ship.ship_pass[strlen(ship.ship_pass) - 1] = (char)NULL;
                 }
                 ship.ship_pass[strlen(ship.ship_pass) - 1] = (char)NULL;
                 break;

        case 15: printf("Ship warp drive capacity %d\n", ship.ship_warp);
case_15_again:
                 printf("\nEnter new warp class: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_warp;

                 if (int_item > 100) {
                     printf("Maximum warp is 100!\n");
                     goto case_15_again;
                 }
                 ship.ship_warp = int_item;
                 break;

        case 16: printf("Ship torps %d\n", ship.ship_torp);
case_16_again:
                 printf("\nEnter new number of torps: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.ship_torp;

                 if (long_item > 5000) {
                     printf("Maximum torp is 5000!\n");
                     goto case_16_again;
                 }
                 ship.ship_torp = long_item;
                 break;

        case 17: printf("Base credits %ld\n", ship.base_credits);
                 printf("\nEnter new base credits: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_credits;

                 ship.base_credits = long_item;
                 break;

        case 18: printf("Base cargo %ld\n", ship.base_cargo);
                 printf("\nEnter new base cargo: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_cargo;

                 ship.base_cargo = long_item;
                 break;

        case 19: printf("Base crew %ld\n", ship.base_crew);
                 printf("\nEnter new base crew: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_crew;

                 ship.base_crew = long_item;
                 break;

        case 20: printf("Base cloak %d\n", ship.base_cloak);
case_20_again:
                 printf("\nEnter new base cloak class: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.base_cloak;

                 if (int_item > 200) {
                     printf("Maximum base cloak is 200!\n");
                     goto case_20_again;
                 }
                 ship.base_cloak = int_item;
                 break;

        case 21: printf("Base shield %ld\n", ship.base_shield);
                 printf("\nEnter new base shield: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.base_shield;

                 ship.base_shield = long_item;
                 break;

        case 22: printf("   Remote robot sensors editing sub-menu\n");
                 sub_menu_remotes();
                 break;

        case 23: printf("Ships total kills value %d\n", ship.total_kills);
                 printf("\nEnter new total kills: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.total_kills;

                 ship.total_kills = int_item;
                 break;

        case 24: printf("Players time remaining %d\n", ship.time_remaining);
                 printf("\nEnter new time remaining: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.time_remaining;

                 ship.time_remaining = int_item;
                 break;

        case 25: printf("Planets owned %d\n", ship.planets_owned);
                 printf("\nPlanets owned may not be edited. Hit [ENTER] to continue\n");
                 fgets(record, 200, stdin);
                 break;

        case 26: printf("Plague flag %d\n", ship.plague_flag);
                 printf("\nEnter new plague value: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.plague_flag;

                 ship.plague_flag = int_item;
                 break;

        case 27: printf("Ship infected flag %d\n", ship.ship_infected);
                 printf("\nEnter new infection value: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_infected;

                 ship.ship_infected = int_item;
                 break;

        case 28: printf("Ship morale factor %d\n", ship.ship_morale);
                 printf("\nEnter new morale factor: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.ship_morale;

                 ship.ship_morale = int_item;
                 break;

        case 29: printf("   Alliance editing sub-menu\n");
                 sub_menu_alliance();
                 break;

        case 30: printf("Taxes owed to the cops %f\n", ship.taxes);
                 printf("\nEnter new taxes owed: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.taxes;

		 ship.taxes = long_item;
                 break;

        case 31: printf("Impounding warning count %d\n", ship.tax_warnings);
                 printf("\nEnter new impound warning: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.tax_warnings;

                 ship.tax_warnings = int_item;
                 break;

        case 32: printf("   Attack sled editing sub-menu\n");
                 sub_menu_swarm();
                 break;

        case 33: printf("   Last attacked/attacked-by/torp and phaser count sub-menu\n");
                 sub_menu_last();
                 break;

        case 34: printf("How many times logged in %d\n", ship.log_count);
                 printf("\nEnter new times signed-in: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.log_count;

                 ship.log_count = int_item;
                 break;

        case 35: printf("Base destroyed by %s\n", ship.base_death);
                 printf("\nEnter new name of ship that destroyed base: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     ucase(record);
                     if (strlen(record) != 5) {
                         strcpy(ship.base_death, "NONE");
                         printf("Base destroyed set to: NONE\n");
                     }
                     else {
                         STRNCPY(ship.base_death, record, 4);
                     }
                 }
                 break;

        case 36: printf("Base hit count %d\n", ship.base_hit_count);
                 printf("\nBase hit count may not be edited! Hit [ENTER] to continue: ");
                 fgets(record, 200, stdin);
                 break;

        case 37: printf("   NAV planet and point-in-space editing sub-menu\n");
                 sub_menu_nav();
                 break;

        case 38: if (ship.outstanding_bid != (char)NIL) {
                     printf("Outstanding bid on ship number %d\n",
                         ship.outstanding_bid);
                 }
                 else {
                     printf("Ship has no outstanding bid.\n");
                 }
case_38_again:
                 printf("\nEnter new outstanding ship number (Enter 0 for none): ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.outstanding_bid;

                 if (int_item == 0) {
                     ship.outstanding_bid = (char)NIL;
                     break;
                 }

                 if (int_item < 0 || int_item > total_players) {
                     printf("Ship number is invalid!\n");
                     goto case_38_again;
                 }
		 ship.outstanding_bid = int_item;
                 break;

        case 39: printf("Bid amount: %ld\n", ship.bid_amount);
                 printf("\nEnter new amount of bid: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.bid_amount;

                 ship.bid_amount = long_item;
                 break;

        case 40: printf("Base last boarded by %s\n", ship.base_boarded);
                 printf("\nEnter new last base boarded by: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     ucase(record);
                     if (strlen(record) != 5) {
                         strcpy(ship.base_boarded, "NONE");
                         printf("Base boarded set to: NONE\n");
                     }
                     else {
                         STRNCPY(ship.base_boarded, record, 4);
                     }
                 }
                 break;

        case 41: printf("   Taxes owed to planets editing sum-menu\n");
                 sub_menu_tax_planet();
                 break;

        case 42: printf("   Owned planets editing sub-menu\n");
                 printf("\nPlanets owned may not be edited. Hit [ENTER] to continue\n");
                 fgets(record, 200, stdin);
                 break;

        case 43: printf("Sick bay has %d crew members in it\n", ship.sick_bay);
                 printf("\nEnter new total in sick bay: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.sick_bay;

                 ship.sick_bay = int_item;
                 break;

        case 44: printf("Bounty on this ship %ld\n", ship.bounty);
                 printf("\nEnter new bounty value: ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     long_item = atol(record);
                 else
                     long_item = ship.bounty;

                 ship.bounty = long_item;
                 break;

        case 45: printf("Who destroyed this ship %s\n", ship.who_destroyed);
                 printf("\nEnter new ships name: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     ucase(record);
                     if (strlen(record) != 5) {
                         strcpy(ship.who_destroyed, "NONE");
                         printf("Who destroyed set to: NONE\n");
                     }
                     else {
                         STRNCPY(ship.who_destroyed, record, 4);
                     }
                 }
                 break;

        case 46: printf("   Scout ship editing sub-menu\n");
                 sub_menu_scout();
                 break;

        case 47: printf("Pirate authorization access code: %s\n", ship.pirate_code);
                 printf("\nEnter new authorization code: ");
                 fgets(record, 200, stdin);
                 if (strlen(record) != 1) {
                     STRNCPY(ship.pirate_code, record, 29);
                     ship.pirate_code[strlen(ship.pirate_code) - 1] = (char)NULL;
                 }
                 break;

        case 48: if (ship.leashed_by == (char)NIL) {
                     printf("This ship is not leashed by any other ship.\n");
                 }
                 else {
                     printf("Leashed by ship number %d\n", ship.leashed_by);
                 }
case_48_again:
                 printf("\nEnter new ship number (Enter 0 for none): ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.leashed_by;

                 if (int_item == 0) {
                     ship.leashed_by = (char)NIL;
                     break;
                 }

                 if (int_item < 0 || int_item > total_players) {
                     printf("Ship number is invalid!\n");
                     goto case_48_again;
                 }
                 ship.leashed_by = int_item;
                 break;

        case 49: if (ship.leashed_to == (char)NIL) {
                     printf("This ship is not leashed to anything.\n");
                 }
                 else {
                     printf("Leashed to ship number %d\n", ship.leashed_to);
                 }
case_49_again:
                 printf("\nEnter new ship number (Enter 0 for no leash): ");
                 fgets(record, 200, stdin);

                 if (strlen(record) != 1)
                     int_item = atoi(record);
                 else
                     int_item = ship.leashed_to;

                 if (int_item == 0) {
                     ship.leashed_to = (char)NIL;
                     break;
                 }

                 if (int_item < 0 || int_item > total_players) {
                     printf("Ship number is invalid!\n");
                     goto case_49_again;
                 }
                 ship.leashed_to = int_item;
                 break;

        case 50: printf("This is a %s ship\n", ship.local  == 0 ? "LOCAL" : "REMOTE");
                 if (ship.local == 0) {
                     printf("Swapped to a remote player\n");
		     ship.local = 1;
                 }
                 else {
                     printf("Swapped to a local player\n");
		     ship.local = 0;
                 }
                 break;

        case 51: printf("Keyboard Macros may not be edited here.\n");
                 break;
    }

    write_record(current_record);
}

/* **********************************************************************
   * Display all ships elements.                                        *
   *                                                                    *
   ********************************************************************** */

static void display_ship_elements(void)
{
    char item_number;
        
restart_record:
    clrscr();
    printf(" 1) Ships name: %s\n", ship.ship_name);
    printf(" 2) Captains name: %s\n", ship.ship_person);
    printf(" 3) Ships location {%ld-%ld} Universe %d\n", ship.ship_xpos, ship.ship_ypos, ship.ship_universe);
    printf(" 4) Base location {%ld-%ld} Universe %d\n", ship.base_xpos, ship.base_ypos, ship.base_universe);
    printf(" 5) Ship power %ld\n", ship.ship_power);
    printf(" 6) Ship shield %ld\n", ship.ship_shield);
    printf(" 7) Ship credits %ld\n", ship.ship_credits);
    printf(" 8) Ship cargo %d\n", ship.ship_cargo);
    printf(" 9) Ship crew %d\n", ship.ship_crew);
    printf("10) Ship shuttle class %d\n", ship.ship_shuttle);
    printf("11) Ship hull level %d\n", ship.ship_hull);
    printf("12) Ship cloak class %d\n", ship.ship_cloak);
    printf("13) Ship sensor class %d\n", ship.ship_sensor);
    printf("14) Ship password %s\n", ship.ship_pass);
    printf("15) Ship warp drive capacity %d\n", ship.ship_warp);
    printf("16) Ship torps %d\n", ship.ship_torp);
    printf("17) Base credits %ld\n", ship.base_credits);
    printf("18) Base cargo %ld\n", ship.base_cargo);
    printf("19) Base crew %ld\n", ship.base_crew);
    printf("20) Base cloak %d\n", ship.base_cloak);

    gotoxy(1, 22);
    printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
    printf("³ Enter item number, (N)ext screen, (R)restart, (E)nd:        ³\n");
    printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
    gotoxy(56, 23);
    fgets(record, 200, stdin);
    ucase(record);

    if (record[0] == 'R') goto restart_record;
    if (record[0] == 'E') return;

    if (record[0] != 'N' && strlen(record) > 0) {
        item_number = atoi(record);
	if (item_number > 0 && item_number < 21) {
            edit_element(item_number);
            goto restart_record;
        }
    }

    clrscr();
    printf("21) Base shield %ld\n", ship.base_shield);
    printf("22)    Remote robot sensors editing sub-menu\n");
    printf("23) Ships total kills value %d\n", ship.total_kills);
    printf("24) Players time remaining %d\n", ship.time_remaining);
    printf("25) Planets owned %d\n", ship.planets_owned);
    printf("26) Plague flag %d\n", ship.plague_flag);
    printf("27) Ship infected flag %d\n", ship.ship_infected);
    printf("28) Ship morale factor %d\n", ship.ship_morale);
    printf("29)    Alliance editing sub-menu\n");
    printf("30) Taxes owed to the cops %f\n", ship.taxes);
    printf("31) Impounding warning count %d\n", ship.tax_warnings);
    printf("32)    Attack sled editing sub-menu\n");
    printf("33)    Last attacked/attacked-by/torp and phaser count sub-menu\n");
    printf("34) How many times logged in %d\n", ship.log_count);
    printf("35) Base destroyed by %s\n", ship.base_death);
    printf("36) Base hit count %d\n", ship.base_hit_count);
    printf("37)    NAV planet and point-in-space editing sub-menu\n");
    printf("38) Outstanding bid on ship number %d\n", ship.outstanding_bid);
    printf("39) Bid amount: %ld\n", ship.bid_amount);
    printf("40) Base last boarded by %s\n", ship.base_boarded);

    gotoxy(1, 22);
    printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
    printf("³ Enter item number, (N)ext screen, (R)restart, (E)nd:        ³\n");
    printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
    gotoxy(56, 23);
    fgets(record, 200, stdin);
    ucase(record);

    if (record[0] == 'R') goto restart_record;
    if (record[0] == 'E') return;

    if (record[0] != 'N' && strlen(record) > 0) {
        item_number = atoi(record);
	if (item_number > 20 && item_number < 41) {
            edit_element(item_number);
            goto restart_record;
        }
    }

    clrscr();
    printf("41)    Taxes owed to planets editing sum-menu\n");
    printf("42)    Owned planets editing sub-menu\n");
    printf("43) Sick bay has %d crew members in it\n", ship.sick_bay);
    printf("44) Bounty on this ship %ld\n", ship.bounty);
    printf("45) How destroyed this ship %s\n", ship.who_destroyed);
    printf("46)    Scout ship editing sub-menu\n");
    printf("47) Pirate authorization access code: %s\n", ship.pirate_code);
    printf("48) Leashed by ship number %d\n", ship.leashed_by);
    printf("49) Leashed to ship number %d\n", ship.leashed_to);
    printf("50) This is a %s ship\n", ship.local == 0 ? "LOCAL" : "REMOTE");
    printf("51) Keyboard Macros\n");

    gotoxy(1, 22);
    printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
    printf("³ Enter item number, (R)estart, (E)nd:                        ³\n");
    printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
    gotoxy(45, 23);
    fgets(record, 200, stdin);
    ucase(record);

    if (record[0] == 'R') goto restart_record;
    if (record[0] == 'E') return;

    if (strlen(record) > 0) {
        item_number = atoi(record);
        if (item_number > 40 && item_number < 52) {
            edit_element(item_number);
            goto restart_record;
        }
    }

    goto restart_record;
}

/* **********************************************************************
   * Display the ships and then see if one should be edited.            *
   *                                                                    *
   ********************************************************************** */

static void edit_ship_file(void)
{

ask_for_ship:
    clrscr();
    display_all_ships();
    gotoxy(1, 22);
    printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
    printf("³ Enter record number to edit or [ENTER] to end program:      ³\n");
    printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
    gotoxy(58, 23);
    fgets(record, 200, stdin);
    if (strlen(record) == 1) return; /* Single Carriage return yields 1 */
    current_record = atoi(record);

    if (current_record > total_players) {
	gotoxy(1, 22);
	printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
        printf("³ That record number is not valid. Hit [ENTER] to continue:   ³\n");
	printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
	gotoxy(65, 23);
        fgets(record, 200, stdin);
        goto ask_for_ship;
    }

    read_record(current_record);

    if (! strcmp(ship.ship_name, "NONE")) {
	gotoxy(1, 22);
	printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
	printf("³ That record is marked as empty. Hit [ENTER] to continue:    ³\n");
	printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
	gotoxy(65, 23);
        fgets(record, 200, stdin);
        goto ask_for_ship;
    }

    display_ship_elements();
    goto ask_for_ship;
}

/* **********************************************************************
   * Here is the main entry point.                                      *
   *                                                                    *
   ********************************************************************** */

void main(void)
{
    long itstime;
    char count;

    time(&itstime);
    STRNCPY(the_date, ctime(&itstime), 26);

    if ((config = fopen("SHIP.CFG", "r")) == (FILE *)NULL) {
        printf("I was unable to open file SHIP.CFG!\n");
        exit(0);
    }

    xsize = extract_config();
    ysize = extract_config();
    cfg_string();           /* password                 */
    time_remaining = extract_config();
    cfg_string();           /* starting time            */
    cfg_string();           /* ending time              */

    total_players = extract_config();
    fclose(config);

    if ((ship_file = fopen("SHIP.DAT", "r+b")) == (FILE *)NULL) {
	printf("I was unable to open file SHIP.DAT!\n");
        exit(0);
    }

    for (count = 0; count < 250; count++)
        modified[count] = FALSE;

    clrscr();
    printf("   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
    printf("   ³                                                                    ³\n");
    printf("   ³ UM-EDIT.EXE - Copyright Fredric L. Rice, 1991. All rights reserved ³\n");
    printf("   ³                                                                    ³\n");
    printf("   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
    printf("   ³                                                                    ³\n");
    printf("   ³ This program works with Universal Mayhem Version 2.3 or higher!    ³\n");
    printf("   ³ If you run it on version 2.0 or 2.2 it will report every record    ³\n");
    printf("   ³ as bad and trash your SHIP.DAT file as it attempts to correct      ³\n");
    printf("   ³ what it thinks are corrupted records.                              ³\n");
    printf("   ³                                                                    ³\n");
    printf("   ³ Do you wish to continue? N                                         ³\n");
    printf("   ³                                                                    ³\n");
    printf("   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n");
    gotoxy(31, 12);
    fgets(record, 200, stdin);
    ucase(record);

    if (record[0] == 'Y') {
        clrscr();
        printf("   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
        printf("   ³                                                                    ³\n");
        printf("   ³ UM-EDIT.EXE - Copyright Fredric L. Rice, 1991. All rights reserved ³\n");
        printf("   ³                                                                    ³\n");
        printf("   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        printf("   ³                                                                    ³\n");
        printf("   ³            Validate of SHIP.DAT file currently under way.          ³\n");
        printf("   ³                       Validate %04d records                        ³\n", total_players);
        printf("   ³                                                                    ³\n");
        printf("   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n");

        validate_ship_file();
        clrscr();
        edit_ship_file();

        clrscr();
        printf("   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
        printf("   ³                                                                    ³\n");
        printf("   ³ UM-EDIT.EXE - Copyright Fredric L. Rice, 1991. All rights reserved ³\n");
        printf("   ³                                                                    ³\n");
        printf("   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        printf("   ³                                                                    ³\n");
        printf("   ³    If you have any questions or problems, please let me know:      ³\n");
        printf("   ³           Fredric L. Rice, 1:102/901.0  1-818-335-0128             ³\n");
        printf("   ³                                                                    ³\n");
        printf("   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n");
    }
    else {
        clrscr();
        printf("   ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
        printf("   ³                                                                    ³\n");
        printf("   ³ UM-EDIT.EXE - Copyright Fredric L. Rice, 1991. All rights reserved ³\n");
        printf("   ³                                                                    ³\n");
        printf("   ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
        printf("   ³                                                                    ³\n");
        printf("   ³                 Validation and edit not performed                  ³\n");
        printf("   ³                                                                    ³\n");
        printf("   ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n\n");
    }

    fcloseall();
}


