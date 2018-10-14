
/* **********************************************************************
   * umremote.c                                                         *
   *									*
   * Copyrite (C) 1990, 1991.                                           *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   ********************************************************************** */

/* **********************************************************************
   * Before we include color.c, let's define some prototypes.		*
   *									*
   ********************************************************************** */

   static void send_string(char *this_string);

#include "dir.h"
#include "bios.h"
#include "process.h"
#include "dos.h"
#include "stdlib.h"
#include "defines.h"
#include "stdio.h"

/* **********************************************************************
   * The message file format offered here is Fido format which has	*
   * been tested with OPUS and Dutchie. It represents the latest	*
   * format that I know about.						*
   *									*
   ********************************************************************** */

   struct fido_msg {
      char from[36];		/* Who the message is from		  */
      char to[36];		/* Who the message to to		  */
      char subject[72];		/* The subject of the message.		  */
      char date[20];		/* Message createion date/time		  */
      int times;		/* Number of time the message was read	  */
      int destination_node;	/* Intended destination node		  */
      int originate_node;	/* The originator node of the message	  */
      int cost;			/* Cost to send this message		  */
      int originate_net;	/* The originator net of the message	  */
      int destination_net;	/* Intended destination net number	  */
      int destination_zone;	/* Intended zone for the message	  */
      int originate_zone;	/* The zone of the originating system	  */
      int destination_point;	/* Is there a point to destination?	  */
      int originate_point;	/* The point that originated the message  */
      unsigned reply;		/* Thread to previous reply		  */
      unsigned attribute;	/* Message type				  */
      unsigned upwards_reply;	/* Thread to next message reply		  */
      char a_character_1;	/* Used for AREA:MAYHEM precursor	  */
      char area_name[11];	/* Echo mail name			  */
   } message;			/* Something to point to this structure	  */

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
} ;                         /* active ship                                */

   static struct old_ship_file oenemy, oships;

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
} ;                         /* active ship                                */

   static struct new_ship_file nenemy, nships;

/* **********************************************************************
   * Define the variables that are to be used.                          *
   *                                                                    *
   ********************************************************************** */

   static struct ffblk file_block;
   static char mayhem_return;
   static char port_assignment;
   static FILE *config;
   static char record[201];
   static char is_redirected;
   static char want_color;
   static char color_enable;
   static FILE *user_file;
   static long check_address;
   static char statistics_inbound_directory[70];
   static long xsize, ysize;
   static char at_least_one;
   static char diagnostic;
   static char skip_backup;
   static char running_new;

   static long arandom(long low, long high);

/* **********************************************************************
   * Here are the states of the check_this_user() return values.	*
   *									*
   ********************************************************************** */

   enum Check_States {
      Exists,		/* The user already exists 			*/
      Update,		/* Update of an existing remote player.		*/
      New_One,          /* A new addition to the player file.           */
      Destroyed,        /* The imported ship was destroyed already.     */
      Corrupted,        /* Ships name was corrupted or something bad.   */
      Weaker		/* Update not done because it was weaker ship.	*/
   } ;

/* **********************************************************************
   * See if we should abort the run.					*
   *									*
   ********************************************************************** */

static void check_for_abort(void)
{
    unsigned char byte;

    if (kbhit() != 0) {
	byte = getch();
	if (byte == 27) {
            printf("\nOperator aborted\n");
	    fcloseall();
	    exit(0);
	}
    }
}

/* **********************************************************************
   * Search user file for the offered ships name. 			*
   *									*
   * o If the name doesn't exist, return New_One.                       *
   *									*
   * o If the ship does exist,  check to see if the ship is local or    *
   *   remote. If it's a remote player, return Update.                  *
   *									*
   * o Otherwise, return Exists.                                        *
   *									*
   * o An addition: If the ship is destroyed, return Destroyed.         *
   *                                                                    *
   ********************************************************************** */

static char check_this_user(char *this_ship, char file_type)
{
   unsigned long total_power1, total_power2;
   char name_test, result;

   if (! file_type) {
      if (oenemy.ship_xpos == 0 && oenemy.ship_ypos == 0) return(Destroyed);
   }
   else {
      if (nenemy.ship_xpos == 0 && nenemy.ship_ypos == 0) return(Destroyed);
   }

   if (! file_type) {
      if (strlen(oenemy.ship_name) != 4) return(Corrupted);
   }
   else {
      if (strlen(nenemy.ship_name) != 4) return(Corrupted);
   }

   for (name_test = 0; name_test < 4; name_test++) {
      if (! file_type) {
         if (oenemy.ship_name[name_test] < 'A' || oenemy.ship_name[name_test] > 'Z') {
            if (oenemy.ship_name[name_test] < '0' || oenemy.ship_name[name_test] > '9') {
               return(Corrupted);
            }
         }
      }
      else {
         if (nenemy.ship_name[name_test] < 'A' || nenemy.ship_name[name_test] > 'Z') {
            if (nenemy.ship_name[name_test] < '0' || nenemy.ship_name[name_test] > '9') {
               return(Corrupted);
            }
         }
      }
   }

   rewind(user_file);

   if (! file_type) {
      total_power1 = oenemy.ship_power +
                     oenemy.ship_shield +
                     oenemy.base_shield;
   }
   else {
      total_power1 = nenemy.ship_power +
                     nenemy.ship_shield +
                     nenemy.base_shield;
   }

   while (! feof(user_file)) {
      check_for_abort();
      check_address = ftell(user_file);

      if (! running_new)
         result = fread(&oships, sizeof(struct old_ship_file), 1, user_file);
      else
         result = fread(&nships, sizeof(struct new_ship_file), 1, user_file);

      if (result == 1) {
         if (! running_new) {
            result = strncmp(oships.ship_name, this_ship, 4);
         }
         else {
            result = strncmp(nships.ship_name, this_ship, 4);
         }
         if (! result) {
            if (! running_new) {
               if (oships.local == 0) {
                  return(Exists);
               }
               else {
                  total_power2 = oships.ship_power +
                                 oships.ship_shield +
                                 oships.base_shield;
               }
            }
            else {
               if (nships.local == 0) {
                  return(Exists);
               }
               else {
                  total_power2 = nships.ship_power +
                                 nships.ship_shield +
                                 nships.base_shield;
               }
            }
            if (total_power1 > total_power2) {
               return(Update);
            }
            else {
               return(Weaker);
	    }
	 }
      }
      else {
	 return(New_One);
      }
   }

   return(New_One);
}

/* **********************************************************************
   * Search through the user data file until an empty slot is found.	*
   *									*
   * Return TRUE if there is one, else return FALSE.			*
   *									*
   ********************************************************************** */

static char posistion_to_first_available_slot(void)
{
   long this_point;
   char result;

   rewind(user_file);

   while (! feof(user_file)) {
      this_point = ftell(user_file);

      if (! running_new) {
         result = fread(&oships, sizeof(struct old_ship_file), 1, user_file);
      }
      else {
         result = fread(&nships, sizeof(struct new_ship_file), 1, user_file);
      }

      if (result == 1) {
         if (! running_new) {
            result = strncmp(oships.ship_name, "NONE", 4);
         }
         else {
            result = strncmp(nships.ship_name, "NONE", 4);
         }
         if (! result) {
	    fseek(user_file, this_point, SEEK_SET);
	    return(TRUE);
	 }
      }
      else {
	 return(FALSE);
      }
   }

   return(FALSE);
}

/* **********************************************************************
   * See if we have carrier.                                            *
   *									*
   ********************************************************************** */

static char check_carrier_detect(void)
{
   int status;
   char return_value;

   status = bioscom(3, 0, port_assignment);
   return_value = (status & 0x0080) == 0x0080;
   return(return_value);
}

/* **********************************************************************
   * Send the character waiting to get out.                             *
   *                                                                    *
   * At the same time, give it to the console.                          *
   *                                                                    *
   ********************************************************************** */

static void send_character(char this_byte)
{
   if (is_redirected) bioscom(1, this_byte, port_assignment);
   putchar(this_byte);
}

/* **********************************************************************
   * Send the string out the com port.                                  *
   *                                                                    *
   ********************************************************************** */

static void send_string(char *this_string)
{
   while (*this_string)
      send_character(*this_string++);
}

/* **********************************************************************
   * Extract the command file from the attach file.                     *
   *                                                                    *
   * Keep what's in it if the make_file is TRUE.                        *
   *                                                                    *
   * Else throw away the command file information.                      *
   *                                                                    *
   * The end of command file is marked with a char(123).                *
   *                                                                    *
   ********************************************************************** */

static char extract_command_file(char *ships_name,
    FILE *attached,
    char make_file)
{
    long current_point;
    FILE *command;
    char file_name[40];
    char found_any;

    found_any = FALSE;

    check_for_abort();

    if (make_file) {
        strcpy(file_name, ships_name);
        strcat(file_name, ".SHP");
	if ((command = fopen(file_name, "wt")) == (FILE *)NULL) {
            if (diagnostic) {
                printf("DIAGNOSTIC: File %s can't be made!\n",
                    file_name);
            }
            return(FALSE);
	}
        if (diagnostic) {
            printf("DIAGNOSTIC: File %s has been made.\n", file_name);
        }
    }

    if (diagnostic) {
        printf("DIAGNOSTIC: Creating command file contents for ship\n");
    }

    while (! feof(attached)) {
	check_for_abort();
        current_point = ftell(attached);
        fgets(record, 200, attached);
        if (! feof(attached)) {
	    if (record[0] == 123) {
                fseek(attached, current_point + 1, SEEK_SET);
                if (make_file) {
                    fclose(command);
                }
                if (diagnostic) {
                    printf("DIAGNOSTIC: End of command file.\n");
                }
                return(TRUE);
            }
            if (make_file) {
                found_any = TRUE;
		fputs(record, command);
            }
        }
    }

    if (make_file) {
        fclose(command);
    }

    if (diagnostic) {
        printf("DIAGNOSTIC: End of import file. Found any? %s\n",
            found_any ? "YES" : "NO");
    }

    return(found_any);
}

/* **********************************************************************
   * Reposistion things in the new universe if they don't fit in the    *
   * new one.                                                           *
   *                                                                    *
   ********************************************************************** */

static void update_enemy_locations(char file_type)
{
    char aloop;

    if (diagnostic) {
        printf("DIAGNOSTIC: Update enemy ships locations...\n");
    }

    if (! running_new) {
        if (oenemy.ship_xpos >= xsize)
            oenemy.ship_xpos = arandom(10L, xsize - 10);

        if (oenemy.ship_ypos >= ysize)
            oenemy.ship_ypos = arandom(10L, ysize - 10);

        if (oenemy.base_xpos >= xsize)
            oenemy.base_xpos = arandom(10L, xsize - 10);

        if (oenemy.base_ypos >= ysize)
            oenemy.base_ypos = arandom(10L, ysize - 10);

        for (aloop = 0; aloop < 10; aloop++) {
            if (oenemy.rem_xpos[aloop] >= xsize) {
                oenemy.rem_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (oenemy.rem_ypos[aloop] >= ysize) {
                oenemy.rem_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 15; aloop++) {
            if (oenemy.sled_xpos[aloop] >= xsize) {
                oenemy.sled_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (oenemy.sled_ypos[aloop] >= ysize) {
                oenemy.sled_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 100; aloop++)
            oenemy.owned_planets[aloop] = (short)NIL;

        for (aloop = 0; aloop < 10; aloop++) {
            if (oenemy.scout_xpos[aloop] >= xsize) {
                oenemy.scout_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (oenemy.scout_ypos[aloop] >= ysize) {
                oenemy.scout_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 10; aloop++) {
            if (oenemy.scout_to_x[aloop] >= xsize) {
                oenemy.scout_to_x[aloop] = arandom(10L, xsize - 10);
            }
            if (oenemy.scout_to_y[aloop] >= xsize) {
                oenemy.scout_to_y[aloop] = arandom(10L, ysize - 10);
            }
        }
    }
    else {
        if (nenemy.ship_xpos >= xsize)
            nenemy.ship_xpos = arandom(10L, xsize - 10);

        if (nenemy.ship_ypos >= ysize)
            nenemy.ship_ypos = arandom(10L, ysize - 10);

        if (nenemy.base_xpos >= xsize)
            nenemy.base_xpos = arandom(10L, xsize - 10);

        if (nenemy.base_ypos >= ysize)
            nenemy.base_ypos = arandom(10L, ysize - 10);

        for (aloop = 0; aloop < 10; aloop++) {
            if (nenemy.rem_xpos[aloop] >= xsize) {
                nenemy.rem_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (nenemy.rem_ypos[aloop] >= ysize) {
                nenemy.rem_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 15; aloop++) {
            if (nenemy.sled_xpos[aloop] >= xsize) {
                nenemy.sled_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (nenemy.sled_ypos[aloop] >= ysize) {
                nenemy.sled_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 100; aloop++)
            nenemy.owned_planets[aloop] = (short)NIL;

        for (aloop = 0; aloop < 10; aloop++) {
            if (nenemy.scout_xpos[aloop] >= xsize) {
                nenemy.scout_xpos[aloop] = arandom(10L, xsize - 10);
            }
            if (nenemy.scout_ypos[aloop] >= ysize) {
                nenemy.scout_ypos[aloop] = arandom(10L, ysize - 10);
            }
        }

        for (aloop = 0; aloop < 10; aloop++) {
            if (nenemy.scout_to_x[aloop] >= xsize) {
                nenemy.scout_to_x[aloop] = arandom(10L, xsize - 10);
            }
            if (nenemy.scout_to_y[aloop] >= xsize) {
                nenemy.scout_to_y[aloop] = arandom(10L, ysize - 10);
            }
        }

/*
    If we are running a new system and are importing an old format,
    add in the point tag and the Keyboard Macros.
*/

        if (! file_type) {
            for (aloop = 0; aloop < 20; aloop++) {
                nenemy.point_tag[aloop][0] = '-';
            }
            for (aloop = 0; aloop < MACROS; aloop++) {
                nenemy.macro[aloop][0] = (char)NULL;
            }
        }
    }
}

/* **********************************************************************
   * Open the attached file and extract the player information.		*
   *									*
   ********************************************************************** */

static void extract_player_information(char *name)
{
    char path_name[100];
    FILE *attached;
    int how_many_attached;
    char status, result;
    char file_type;

/*
    File_type is TRUE if it's new format, else FALSE
*/

    file_type = (name[strlen(name) - 1] == 'N');

    strcpy(path_name, statistics_inbound_directory);
    if (path_name[strlen(path_name) - 1] != '\\') strcat(path_name, "\\");
    strcat(path_name, name);

    printf(" --> %s %s format file\n",
        path_name,
        file_type ? "NEW" : "OLD");

    if ((attached = fopen(path_name, "rb")) == (FILE *)NULL) {
        printf("   !!! Can't find file: %s !!!\n", path_name);
	return;
    }

    how_many_attached = fgetc(attached);
    printf(" --> %d records\n", how_many_attached);

    for (; how_many_attached > 0; how_many_attached--) {
	check_for_abort();

        if (! file_type)
            result = fread(&oenemy, sizeof(struct old_ship_file), 1, attached);
        else
            result = fread(&nenemy, sizeof(struct new_ship_file), 1, attached);

        if (result == 1) {
            if (! file_type) {
                printf("     Ship '%s' --> ", oenemy.ship_name);
            }
            else {
                printf("     Ship '%s' --> ", nenemy.ship_name);
            }
	    at_least_one = TRUE;
            if (! file_type) {
                status = check_this_user(oenemy.ship_name, file_type);
            }
            else {
                status = check_this_user(nenemy.ship_name, file_type);
            }

            if (status == Exists) {
                printf("[Already Exists. Rejected]\n");
                if (! file_type) {
                    (void)extract_command_file(oenemy.ship_name, attached, FALSE);
                }
                else {
                    (void)extract_command_file(nenemy.ship_name, attached, FALSE);
                }
            }
            else if (status == Update) {
                printf("[Exists, being updated]\n");
		fseek(user_file, check_address, SEEK_SET);
                update_enemy_locations(file_type);
                if (! running_new) {
                    fwrite(&oenemy, sizeof(struct old_ship_file), 1, user_file);
                }
                else {
                    fwrite(&nenemy, sizeof(struct new_ship_file), 1, user_file);
                }
                if (! file_type) {
                    if (!extract_command_file(oenemy.ship_name, attached, TRUE)) {
                        printf(" !!! Can't update command file !!!\n");
                    }
                }
                else {
                    if (!extract_command_file(nenemy.ship_name, attached, TRUE)) {
                        printf(" !!! Can't update command file !!!\n");
                    }
                }
            }
            else if (status == New_One) {
                if (! posistion_to_first_available_slot()) {
                    printf("[New ship. No room for it!]\n");
                    if (! file_type) {
                        (void)extract_command_file(oenemy.ship_name, attached, FALSE);
                    }
                    else {
                        (void)extract_command_file(nenemy.ship_name, attached, FALSE);
                    }
                }
                else {
                    printf("[Import, new ship]\n");
                    update_enemy_locations(file_type);
                    if (! running_new) {
                        fwrite(&oenemy, sizeof(struct old_ship_file), 1, user_file);
                    }
                    else {
                        fwrite(&nenemy, sizeof(struct new_ship_file), 1, user_file);
                    }
                }
                if (! file_type) {
                    if (! extract_command_file(oenemy.ship_name, attached, TRUE)) {
                        printf(" !!! Can't create command file !!!\n");
                    }
                }
                else {
                    if (! extract_command_file(nenemy.ship_name, attached, TRUE)) {
                        printf(" !!! Can't create command file !!!\n");
                    }
                }
            }
            else if (status == Destroyed) {
                printf("[Already Destroyed. Rejected]\n");
                if (! file_type) {
                    (void)extract_command_file(oenemy.ship_name, attached, FALSE);
                }
                else {
                    (void)extract_command_file(nenemy.ship_name, attached, FALSE);
                }
	    }
	    else if (status == Weaker) {
                printf("[Import ship is weaker. Rejected]\n");
                if (! file_type) {
                    (void)extract_command_file(oenemy.ship_name, attached, FALSE);
                }
                else {
                    (void)extract_command_file(nenemy.ship_name, attached, FALSE);
                }
	    }
            else if (status == Corrupted) {
                printf("[SHIP IS CORRUPTED! Not accepted!]\n");
                if (! file_type) {
                    (void)extract_command_file(oenemy.ship_name, attached, FALSE);
                }
                else {
                    (void)extract_command_file(nenemy.ship_name, attached, FALSE);
                }
            }
        }
    }

    fclose(attached);
    unlink(path_name);
    printf("     Erased %s\n", path_name);
}

/* **********************************************************************
   * This function opens all the message files and checks them to see	*
   * if any are marked as :stat: type messages. If they are any, the	*
   * files are opened up and the remote player data records and command	*
   * files are extracted and placed in the local system.		*
   *									*
   ********************************************************************** */

static void scan_file_directory(void)
{
   char result, file_type;
   char directory_search[100];

   if (statistics_inbound_directory[strlen(statistics_inbound_directory) - 1] != '\\') {
      strcpy(directory_search, statistics_inbound_directory);
      strcat(directory_search, "\\*.MH?");
   }
   else {
      strcpy(directory_search, statistics_inbound_directory);
      strcat(directory_search, "*.MH?");
   }

   if (diagnostic) {
      printf("DIAGNOSTIC: Directory search: %s\n", directory_search);
   }

   result = findfirst(directory_search, &file_block, 0x16);

   if (! result) {
      file_type = file_block.ff_name[strlen(file_block.ff_name) - 1];
      if (file_type == 'M' || file_type == 'N') {
         extract_player_information(file_block.ff_name);
      }
      else {
         printf("--> Skipping %s\n", file_block.ff_name);
      }
   }
   else {
      if (diagnostic) {
         printf("DIAGNOSTIC: No *.MHM or *.MHN files were found!\n");
      }
   }

   while (! result) {
      result = findnext(&file_block);
      if (! result) {
         file_type = file_block.ff_name[strlen(file_block.ff_name) - 1];
         if (file_type == 'M' || file_type == 'N') {
            extract_player_information(file_block.ff_name);
         }
         else {
            printf("--> Skipping %s\n", file_block.ff_name);
         }
      }
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
      fputs("SHIP.CFG is corrupted!\n", stderr);
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
      fputs("SHIP.CFG is corrupted!\n", stderr);
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
   * Generate a random number.                                          *
   *                                                                    *
   ********************************************************************** */

static long arandom(long low, long high)
{
   long the_test;
   short tcount, try_random;

   the_test = 0;
   try_random = 0;

   for (tcount = 0; tcount < 40; tcount++, try_random++) {
      if (high < 5) {
         the_test = rand() / 8000;
      }
      else if (high < 9) {
         the_test = rand() / 4000;
      }
      else if (high < 17) {
         the_test = rand() / 2000;
      }
      else if (high < 32) {
         the_test = rand() / 1000;
      }
      else if (high < 327) {
         the_test = rand() / 100;
      }
      else if (high < 3276) {
         the_test = rand() / 10;
      }
      else if (high < 32766) {
         the_test = rand();
      }
      else if (high < ((long) 32766 * 2)) {
         the_test = (long) rand() * 2;
      }
      else if (high < ((long) 32766 * 3)) {
         the_test = (long) rand() * 3;
      }

      if (the_test > low && the_test < high) {
         return((long) the_test);
      }
   }

   return(low);
}

/* **********************************************************************
   * Here is the main entry point.                                      *
   *                                                                    *
   * Universal Mayhem will invoke this function and pass a string to    *
   * it to let us know we will need to call Universal Mayhem back.      *
   *                                                                    *
   ********************************************************************** */

void main(int argc, char *argv[])
{
   FILE *backup;
   char arg_loop;
   char new_format = FALSE;
   char old_format = FALSE;
   char skip_backup = FALSE;
   char diagnostic = FALSE;
        
   mayhem_return = (argc > 1 && !strncmp(argv[1], "Mayhem", 6));
   want_color = (argc > 2 && argv[2][0] == 'Y');
   color_enable = (argc > 3 && argv[3][0] == 'Y');

   is_redirected = at_least_one = FALSE;

   for (arg_loop = 0; arg_loop < argc; arg_loop++) {
      if (argv[arg_loop][0] == '/') {
         if (toupper(argv[arg_loop][1]) == 'O') {
             old_format = TRUE;
         }
         else if (toupper(argv[arg_loop][1]) == 'N') {
             new_format = TRUE;
         }
         else if (toupper(argv[arg_loop][1]) == 'S') {
             skip_backup = TRUE;
         }
         else if (toupper(argv[arg_loop][1]) == 'D') {
             diagnostic = TRUE;
         }
      }
   }

   if ((config = fopen("SHIP.CFG", "r")) == (FILE *)NULL) {
      printf("Unable to open: SHIP.CFG!\n");
      exit(3);
   }

   xsize = extract_config();
   ysize = extract_config();

   if (diagnostic) {
      printf("DIAGNOSTIC: XSize of universe is %d\n", xsize);
      printf("DIAGNOSTIC: YSize of universe is %d\n", ysize);
   }

   cfg_string();
   (void)extract_config();
   (void)cfg_string();
   (void)cfg_string();
   (void)extract_config();
   cfg_string();
   (void)cfg_string();
   (void)cfg_string();
   port_assignment = extract_config();

   if (diagnostic) {
      printf("\nDIAGNOSTIC: Port assignment is %d\n", port_assignment);
   }
 
   is_redirected = check_carrier_detect();

   if (diagnostic) {
      printf("DIAGNOSTIC: Modem is%s redirected. We %s carrier\n",
         is_redirected ? "" : " NOT", is_redirected ? "HAVE" : "DONT HAVE");
   }

   cfg_string();
   cfg_string();
   cfg_string();
   cfg_string();
   cfg_string();
   cfg_string();
   cfg_string();

   cfg_string();
   ucase(record);
   strncpy(statistics_inbound_directory, record, 65);
   statistics_inbound_directory[65] = (char)NULL;
   fclose(config);

   clrscr();
   printf("ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
   printf("³          Universal Mayhem Remote Player Data File Importer                ³\n");
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
   printf("³         Copyright 1991, Fredric L. Rice. All rights reserved.             ³\n");
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
   printf("³                                                                           ³\n");
   printf("³  This program will work with version 2.0, 2.2, and 2.3 of Universal       ³\n");
   printf("³  Mayhem. You must tell it what version you use. It will import all        ³\n");
   printf("³  *.MHM files as version 2.0 and 2.2 files and all *.MHN as version 2.3    ³\n");
   printf("³  and version 2.4.                                                         ³\n");
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
   printf("³                                                                           ³\n");
   printf("³          UMREMOTE /d      diagnostic displays are offered                 ³\n");
   printf("³          UMREMOTE /s      skips backup of player data file                ³\n");
   printf("³          UMREMOTE /o      Doesn't ask what version and assumes OLD        ³\n");
   printf("³          UMREMOTE /n      Doesn't ask what version and assumes NEW (2.5)  ³\n");
   printf("³                                                                           ³\n");
   printf("ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´\n");
   printf("³ Searching: %63s³\n", statistics_inbound_directory);
   printf("ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");

   if (port_assignment != 0 && port_assignment != 1) {
      printf("\nPort assignment %d not supported!\n", port_assignment);
      exit(0);
   }

   if (! skip_backup) {
      backup = fopen("SHIPDAT.BAK", "rb");

      if (backup != (FILE *)NULL) {
         printf("\nBackup file: SHIPDAT.BAK exists. Should I overwrite it? ");
	 fgets(record, 200, stdin);
	 ucase(record);
	 if (record[0] != 'Y') {
	    exit(0);
	 }
	 fclose(backup);
      }

      printf("\n---> Backup of SHIP.DAT to SHIPDAT.BAK in progress <---\n");

      if ((backup = fopen("SHIPDAT.BAK", "wb")) == (FILE *)NULL) {
         printf("Unable to create backup file SHIPDAT.BAK!\n");
	 exit(3);
      }
   }

   if ((user_file = fopen("SHIP.DAT", "r+b")) == (FILE *)NULL) {
      printf("Unable to file: SHIP.DAT, user file!\n");
      exit(3);
   }

   if (! skip_backup) {
      while (! feof(user_file))
	 fputc(fgetc(user_file), backup);

      fclose(backup);
      rewind(user_file);
   }

ask_version:
   if (old_format || new_format) {
      goto skip_asking_version;
   }
   printf("\n\nAre you running version 2.3 of Mayhem? ");
   fgets(record, 200, stdin);
   ucase(record);
   if (record[0] == 'Y') {
       running_new = TRUE;
   }
   else if (record[0] == 'N') {
       running_new = FALSE;
   }
   else {
       goto ask_version;
   }

skip_asking_version:
   if (new_format) {
      running_new = TRUE;
   }
   else if (old_format) {
      running_new = FALSE;
   }

   if (! mayhem_return) {
      clrscr();
      printf("       ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿\n");
      printf("       ³ Hit [ESC]ape to abort at any time                 ³\n");
      printf("       ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ\n");
   }

   scan_file_directory();

   if (! at_least_one) {
       printf("No files with an .MH? extention were located.\n");
   }

   printf("\nEnd of program.\n");
   fcloseall();
   exit(0);
}

