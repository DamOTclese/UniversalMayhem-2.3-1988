
/* **********************************************************************
   * ship.c		 Main module					*
   *									*
   * "Every body have fun...						*
   *    Every body wang chung."						*
   *									*
   * This is Universal Mayhem, Copyright 1988, 1989, 1990, 1991,        *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   * Universal Mayhem was started January of 1988 and it has already	*
   * gone through MUCH changes.						*
   *									*
   * Universal Mayhem started out as a Digital Researches Compiler	*
   * BASIC program which soon started to exceed the 64K limits of the	*
   * code segment. Conversion over to Turbo C started around March of	*
   * 1988.								*
   *									*
   * Beta testing of the BASIC version was and still is going on in a	*
   * live FidoNet OPUS system, Astro-Net, (103/903). This systems       *
   * people and SysOp has provided a LOT of usefull and very good	*
   * suggestions on how to make the game really worthwile.		*
   *									*
   ********************************************************************** */

#include "leash.h"
#include "stdio.h"
#include "ctype.h"
#include "time.h"
#include "stdlib.h"
#include "alloc.h"
#include "conio.h"
#include "process.h"
#include "dos.h"
#include "io.h"
#include "string.h"
#include "defines.h"
#include "signal.h"
#include "function.h"

/* **********************************************************************
   * Define these variables as external.				*
   *									*
   ********************************************************************** */

   extern short last_priority, decoy_xpos, decoy_ypos, decoy_value;
   extern char page_flag;
   extern char interrupted_serial;
   extern char in_chat;
   extern unsigned long recursed;

/* **********************************************************************
   * Allocate some memory items.					*
   *									*
   ********************************************************************** */

   FILE *config, *aplanets, *auniverse, *aship;
   FILE *ship_std, *msg_file, *acomet, *command_file, *agoals;
   FILE *subspace_messages, *error_log, *ship_log,  *help_file;

   long xsize, ysize, aloop, tloop, dloop, xpos, ypos, count;
   long txpos, typos, itstime, dpoint;
   short players, ship_count, try_count, base_count, remote_count;
   short bail_out, user_number, time_remaining, rpt_loop, the_tech;
   short total_comets;
   static short todays_tax_warning;
   short cost_energy, cost_cargo, cost_shuttle;
   char start_warp, scout_count;
   short infect_count;
   short cost_warp, cost_hull, cost_cloak, cost_crew, cost_sensor;
   short cost_torp, cost_remotes;
   char from_remote;
   short the_rnd, the_remove;
   char suspended_real_time;
   char pirate;
   short pxpos, pypos, cost_sled;
   short swarm_count, control_break;
   char in_test_mode;
   char swarm_test;
   char is_redirected;
   short from_node_zone, from_node_network, from_node_number, from_node_point;
   short to_node_zone, to_node_network, to_node_number, to_node_point;
   short stat_node_zone, stat_node_network, stat_node_number, stat_node_point;
   char port_assignment;
   short game_time_remaining;
   char *record, start_time[6], end_time[6], to_save;
   char ship_name[5];
   static char answer[2];
   char the_date[27], *subspace_mail;
   char the_name[31], allow_mail[4];
   char *point, *old_record, *again_string, did_mail;
   char *go_string, sysop_password[81], goals_count;
   char the_plague, in_sysop;
   char watch_cd, color_enable, want_color;
   char total_pirate_count;
   char password[21];
   float tax_increase;
   unsigned short the_percent, ion_trail, docked, base_docked, ocount;
   unsigned short comet_count;
   unsigned short atpoint, matpoint;
   time_t time1, time2, scout_timer;
   UC zpos, tzpos;
   char valid_command_file, time_warning;
   UL total_allocated, smallest_allocated, largest_allocated;
   char sysop_password_fail;
   char statistics_enabled;
   short active_players;
   char bad_words[21];
   short highest_message_number;
   char *statistics_outbound_directory;
   char *statistics_inbound_directory;
   char *network_directory;
   char stat_hold;
   char *echo_origin;
   char *network_origin;
   char page_end_hour, page_end_minute, page_start_hour, page_start_minute;
   char tandy;
   char files_opened, highest_opened, *file_names[20];
   int ticker;
   void interrupt (*old_interrupt)(void);
   char crash_reset;
   char drag_ship;
   char entering_mail;
   char ham_version;
   char want_test;
   char interrupt_enable, use_drop_file;
   short drop_time_remaining;
   char log_chat;
   char shoved_flag;
   char torped_morale;
   char shield_repair;
   char planet_hit;

/* **********************************************************************
   * This is the structure of the information stored in the SHIP.DAT    *
   * file. This file contains information about everyones ships, bases, *
   * remotes, kills, and everything else.                               *
   *                                                                    *
   * As we put other items into the file, simply append them on.        *
   *                                                                    *
   ********************************************************************** */

struct ship_file {
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
  short owned_planets[OWNABLE]; /* Xpos of 1 of 4 possible ypos of owned  */
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
  char pirate_code[30];	    /* The pirate access code			  */
  char leashed_by;	    /* Ship this one is leased by		  */
  char leashed_to;	    /* Ship this one has got a leash on		  */
  char local;               /* 0 if local, 1 if imported                  */
  char point_tag[20][21];   /* Symbolic name of x/y posistion tag name.   */
  char macro[MACROS][201];  /* Keyboard Macros.                           */
} ;

    struct ship_file *ships, *enemy;

/* **********************************************************************
   * Define the structure of the universe.				*
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
   * 3141, (ships->ship_xpos), and scan the structure to see if any of   *
   * the elements are the same as (ships->ship_ypos). If it were 873,    *
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

    struct universe_file universe;

/* **********************************************************************
   * Here is the planets record structure. There are four of these for	*
   * every xsize in the SHIP.CFG file.					*
   *									*
   * The value 'named[]' MUST be the last item in this structure!	*
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

    struct planets_file planets;

/* **********************************************************************
   * This is the comets structure. There are normally very few of these	*
   * because of the time it takes to scan a section of space.		*
   *									*
   ********************************************************************** */

   struct comets_file {
      unsigned flag : 1;        /* Set TRUE if scanned before else FALSE     */
      unsigned direction : 1;	/* The orbital direction of the comet	     */
      long location[2];	     	/* The x pos and y pos of the comet	     */
      char name[15];	     	/* Name of comet names by first scanned	     */
      char ship[5];             /* Name of the ship who  named the comet     */
   } *comets[40];

/* **********************************************************************
   * Define some storage for the information we pull from every ships   *
   * record. This information is used by most of the routines so let's  *
   * make them global.                                                  *
   *									*
   * By putting the data into a structure and then making TOTAL_PLAYERS *
   * pointersy to the structure, we save memory by using alloc rather   *
   * than simply dimentioning the arrays we need out to TOTAL_PLAYERS.	*
   *									*
   * It's important that it is understood how information from the	*
   * arrays of pointers are extract and allocated. When we read a ship	*
   * record and the name is valid, (that is, not 'NONE'), we ask for a	*
   * block of memory to be allocated with  a size the size of the 	*
   * structure 'holder'. If the memory is available, a pointer is given *
   * to us which points to that block of memory.			*
   *									*
   * We put the pointer to that block into an array and go on to read	*
   * the entire contents of the ships users file.			*
   *									*
   * When we need to extract information from the memory blocks, all we *
   * need to do is point to it with the following like syntax:		*
   *									*
   *              hold[interesting_number]->names			*
   *									*
   * To make sure that we don't try to access unallocated memory, we	*
   * initialize the array of pointers to NULL				*
   *									*
   ********************************************************************** */

    struct holder {
      long sxpos, sypos;		/* Hold ships x and y positions   */
      UC szpos;				/* Ship universe		  */
      long bxpos, bypos;		/* Hold base x and y positions    */
      UC bzpos;				/* Base universe		  */
      short xremotes[10], yremotes[10]; /* Hold remote robots x and y's   */
      UC remote_universe[10];		/* What universe is it in?	  */
      char names[5];                    /* Holds ship name                */
      short standings;                  /* Holds millions of units energy */
      short kills; 	                /* Holds number of kills for ship */
      short xswarm[5], yswarm[5];	/* Positions of attack sleds swrm */
      UC swarm_universe[5];		/* What universe is the swarm in? */
      unsigned is_friendly : 1;         /* TRUE if friendly, else FALSE	  */
   } ;

   struct holder *hold[TOTAL_PLAYERS];

/* **********************************************************************
   * Define a linked list containing structures for the automation of 	*
   * the enemy ships->                                                   *
   *									*
   * The bail-out flags, (four of them), are used to describe these:	*
   *									*
   * Bail-out on power is less than 'bail_1_out_value'			*
   * Bail-out on torp is less than 'bail_2_out_value'			*
   * Bail-out when time remaining is less than 'bail_3_out_value'	*
   * Bail-out when damage received.					*
   *									*
   ********************************************************************** */

    struct command_options {
      short ship;                   /* This is the ship number        */
      unsigned assist : 1;          /* Do we assist the attack?       */
      unsigned defend : 1;          /* Do we defent the attacked?     */
      unsigned attack : 1;          /* Do we attack an enemmy ship?   */
      unsigned run_from : 1;        /* Do we run from a ship?         */
      unsigned decoy : 1;           /* Do we employ decoys?           */
      unsigned bail_1_out : 1;      /* Do we have a bail out class 1? */
      unsigned bail_2_out : 1;      /* Do we have a bail out class 2? */
      unsigned bail_3_out : 1;      /* Do we have a bail out class 3? */
      unsigned bail_4_out : 1;      /* Do we have a bail out class 4? */
      unsigned default_fight : 1;   /* 0-Don't fight, 1-Fight         */
      short decoy_class;            /* What class shuttle for decoy?  */
      UL bail_1_value;              /* When to bail out on power?     */
      short bail_2_value;           /* When to bail out on torps left */
      short bail_3_value;           /* When to bail out on time left  */
      struct command_options *next; /* Pointer for linked list next   */
   } ;

    struct command_options multi;

/* **********************************************************************
   * So that we may find the items easily and know if they are aboard a	*
   * ship, we store the location and a flag in memory.			*
   *									*
   * If 'goal_on_ship' is NIL, then the item is drifting in space. If	*
   * it's anything else, then it's aboard a ship.			*
   *									*
   ********************************************************************** */

   struct goal_elements {
      short goal_xpos;
      short goal_ypos;
      char goal_on_ship;
      UC goal_universe;		/* What universe number is it in	*/
   } ;

   struct goal_elements *goal_item[10], goals;

/* **********************************************************************
   * In order to be able to index into the structure, add, edit, or	*
   * remove the various objects within the linked list, we define some	*
   * pointers. These pointers may be used everywhere so we'll make them	*
   * global.								*
   *									*
   * (I might add that these same methods used within to access the	*
   * structure linked lists were developed at work in real-life		*
   * applications and simply ported over to Universal Mayhem).		*
   *									*
   ********************************************************************** */

    struct command_options *m_first, *m_last, *m_point, *m_next;
    struct command_options *m_test, *m_run, *m_previous;
    unsigned short close_ship[TOTAL_PLAYERS];
    unsigned short close_base[TOTAL_PLAYERS];
    unsigned short close_remotes[TOTAL_PLAYERS];
    unsigned short close_swarms[TOTAL_PLAYERS];
    unsigned short close_goals[10];
    unsigned short close_scouts[10];
    long ion_field[ION_COUNT][2];

/* **********************************************************************
   * For an explanation of this array, if you cant immediatly see whats *
   * happening and how it is used to determine symbolic movement within *
   * the grid, read FidoNews article "Two and Three Dimentional		*
   * Numerical Movements Within a Symbolic Model".			*
   *									*
   ********************************************************************** */

    short directions[18] = {
      -1, -1, -1, 0, -1, 1,
       0, -1,  0, 0,  0, 1,
       1, -1,  1, 0,  1, 1
   };

/* **********************************************************************
   * Allocate 10 messages to randomly issue when a planet gets killed.	*
   *									*
   ********************************************************************** */

    char *dead_planet[] = {
      "Sensors report no life on the planet!",
      "The planet is now a boiling sea of mud!",
      "That planet wasn't very productive anyways...",
      "They deserved what they got on the planet!",
      "That solves the fuel shortage problem on that planet!",
      "You just wiped out seven billion people!",
      "You're an EVIL person for destroying that planet!",
      "They didn't even get a chance to send off an SOS!",
      "What a bummer! Relatives can come home to a burned dinner!",
      "The planet is now a chunk of wet charcol!",
      "The atmosphere is peeling away into space!",
      "The planets moon is spiraling off into outer space!",
      "The planets seas are boiling!",
      "Chock up another ten billion casualties!",
      "Hey! My mother lived their!",
      (char *)NULL
   } ;

/* **********************************************************************
   * Allocate 10 messages to randomly display when a ship is destroyed	*
   *									*
   ********************************************************************** */

    char *dead_ship[] = {
      "Well, it looks like you get to start over again!",
      "Well, you certantly deserved it!",
      "Another widow! You should have never gone to space!",
      "What a bummer! Get back in and exact revenge!",
      "It's time to really get going and DO something about it!",
      "Can't say you wern't warned...",
      "Why did you get destroyed? You're supposed to do in others!",
      "That sure didn't look good!",
      "Your ship is a radioactive mass of spreading dust particles!",
      "Your ship is a hulled-out hazard to navagation!",
      "That's not the way to win a fight!",
      "Next time why don't you try to win it?!",
      "Golly... What a bummer!",
      "Kind of makes you want to get even, huh?",
      "It's a rough life out here, isn't it?",
      (char *)NULL
   } ;

   char *restart_ship[] = {
      "It's time to get back in there and exact revenge!",
      "I wouldn't let that captain get away with it...",
      "The decks were awash with the blood of your faithfull crew!",
      "They fought valantly but in the end; well...:",
      "What remains of your ship isn't very pretty!",
      "The explosion was registered on photographic plates around the galaxy!",
      "The fragments are still expanding!",
      "Your ship sure made a mess!",
      "Man what a fight! You wouldn't have believed the blood!",
      "Exact revenge! Kill! Kill! Kill! Find that sucker!",
      "What the hell was that?",
      "Next time, try to hide and look tiny!",
      "Next time, make an alliance with a very, very big friend!",
      "Next time don't step on anyones toes!",
      "You could at least TRY to make a good showing!",
      (char *)NULL
   } ;

/* **********************************************************************
   * Trap the control C'ing that might occur.				*
   *									*
   ********************************************************************** */

void Trap_Control_C(void)
{
   signal(SIGINT, Trap_Control_C);
}

/* **********************************************************************
   * Revector the 18.2 clock tick.                                      *
   *                                                                    *
   ********************************************************************** */

void interrupt new_clock_tick(void)
{
    if (--ticker == 0) {
        outportb(0x64, 0xfe);  /* Keyboard reset */
        while(TRUE) {};
    }
    old_interrupt();
}

/* **********************************************************************
   * Here is the main entry point.					*
   *									*
   ********************************************************************** */

main(int argc, char *argv[])
{
   char already_asked = FALSE;
   time_t time_four, time_five;
   char command_count;
   char *the_pointer;
   char the_hour, the_minute;
   char start_hour, start_minute;
   char end_hour, end_minute;
   short t_start, t_end, t_current;

   signal(SIGINT, Trap_Control_C);
   want_color = FALSE;
   is_redirected = 0;
   tandy = FALSE;
   interrupted_serial = FALSE;

   time(&itstime);
   time(&scout_timer);          /* Scout ship moves     */
   srand((short)itstime);

   for (count = 0; count < TOTAL_PLAYERS; count++)
      hold[count] = (struct holder *)NULL;

   for (count = 0; count < 20; count++)
      file_names[count] = (char *)NULL;

   m_first = m_last = m_point = m_test = m_run = (struct command_options *)NULL;
   user_number = planet_hit = 0;

   config = aplanets = auniverse = aship = ship_std = (FILE *)NULL;
   msg_file = acomet = command_file = agoals = subspace_messages = (FILE *)NULL;
   error_log = ship_log = help_file = (FILE *)NULL;
   total_allocated = smallest_allocated = largest_allocated = (UL)0;
   active_players = files_opened = highest_opened = shield_repair = 0;
   ticker = Reset_Timer * 60 * 18.2;
   in_chat = FALSE;
   shoved_flag = FALSE;
   torped_morale = FALSE;

/*
   Take the current control-break checking value and store it away.
   Then turn it off if it's on.
*/

   control_break = getcbrk();
   if (control_break == 1) setcbrk(0);

/*
   Allocate some buffers
*/

   record = (char *)farmalloc(201);
   old_record = (char *)farmalloc(201);
   again_string = (char *)farmalloc(201);
   go_string = (char *)farmalloc(201);
   memory_allocated(201 * 4);
   subspace_mail = (char *)farmalloc(81);
   memory_allocated(81);
   statistics_outbound_directory = (char *)farmalloc(81);
   memory_allocated(81);
   statistics_inbound_directory = (char *)farmalloc(81);
   memory_allocated(81);
   network_directory = (char *)farmalloc(71);
   memory_allocated(71);
   echo_origin = (char *)farmalloc(66);
   memory_allocated(66);
   network_origin = (char *)farmalloc(66);
   memory_allocated(66);

   *again_string = *go_string = *record = *subspace_mail = (char)NULL;
   *statistics_outbound_directory = *statistics_inbound_directory = (char)NULL;
   *network_directory = *echo_origin = *network_origin = (char)NULL;

   ships = (struct ship_file *)farmalloc(sizeof(struct ship_file));
   memory_allocated(sizeof(struct ship_file));
   enemy = (struct ship_file *)farmalloc(sizeof(struct ship_file));
   memory_allocated(sizeof(struct ship_file));

   if (ships == (struct ship_file *)NULL || enemy == (struct ship_file *)NULL) {
        c_out(LIGHTRED, "Unable to allocate memory for user and enemy!\n\r");
        log_error(144);
        perform_quit(51);
   }

   make_zero_record();

   *enemy = *ships;

   want_test = FALSE;

   for (count = 0; count < argc; count++) {
        if (argv[count][0] == '/') {
            if (toupper(argv[count][1]) == 'T') {
                want_test = TRUE;
                fputs(" -----> Tests will be entertained <-----\n", stderr);
            }
        }
    }

/* **********************************************************************
   * Open up the ships config file and pull out the information we need *
   * to play the game.                                                  *
   *                                                                    *
   ********************************************************************** */

   if ((config = mayhem_fopen("SHIP.CFG", "r", config)) == (FILE *)NULL) {
      config_bad(0);
      log_error(19);
      perform_quit(51);
   }

   xsize = extract_config(1);
   ysize = extract_config(2);

   if (xsize < 1000 || xsize > 32766 || ysize < 1000 || ysize > 32766) {
      c_out(WHITE, "Universe size is %ld by %ld -", xsize, ysize);
      c_out(WHITE, "This is NOT allowed!\n\r");
      c_out(WHITE, "Size must be from 1000 to 32766!\n\r\n\r");
      log_error(0);
      if (crash_reset) {
          setvect(0x1c, old_interrupt);
      }
      if (interrupted_serial) {
         ComClose(port_assignment);
      }
      exit(0);
   }

   cfg_string(3);
   ucase(record);
   strcpy(sysop_password, record);

   if (sysop_password[strlen(sysop_password) - 1] == 0x0a) {
      sysop_password[strlen(sysop_password) - 1] = (char)NULL;
   }

   time_remaining = extract_config(4);

   cfg_string(5);
   strcpy(start_time, record);
   the_pointer = start_time;
   start_hour = atoi(the_pointer);
   the_pointer += 3;
   start_minute = atoi(the_pointer);

   cfg_string(6);
   strcpy(end_time, record);
   the_pointer = end_time;
   end_hour = atoi(the_pointer);
   the_pointer += 3;
   end_minute = atoi(the_pointer);

   players = extract_config(7);

   cfg_string(8);
   ucase(record);
   STRNCPY(subspace_mail, record, 80);
   subspace_mail[80] = (char)NULL;

   cfg_string(9);
   process_origination_network_address();

   cfg_string(10);
   process_destination_network_address();

   port_assignment = extract_config(11);

   if (port_assignment != 0 && port_assignment != 1) {
      c_out(WHITE, "Port assignment %d not supported!\n\r", port_assignment);
      if (crash_reset) {
          setvect(0x1c, old_interrupt);
      }
      if (interrupted_serial) {
         ComClose(port_assignment);
      }
      exit(0);
   }

   cfg_string(12);
   ucase(record);
   STRNCPY(allow_mail, record, 3);

/*
   An important addition to the module was the watching of carrier detect.
   In some multitasking BB systems, the hang up through WATCHCD, (which
   is a Public Domain program), would reset the system, dropping all of
   the people on the system.
*/

   cfg_string(13);
   ucase(record);

   if (! strncmp(record, "YES", 3))
      watch_cd = TRUE;
   else
      watch_cd = FALSE;

/*
   If there is no carrier when we enter this program, we consider the
   program to be running from the operators console
*/

   if (! check_carrier_detect(TRUE))
      is_redirected = 0;
   else
      is_redirected = 1;

/*
   Another addition was the enable of color for the system.
*/

   cfg_string(14);
   ucase(record);

   if (! strncmp(record, "YES", 3))
      color_enable = TRUE;
   else
      color_enable = FALSE;

/*
   The statistics feature was added in August of 1990:
*/

   cfg_string(15);
   ucase(record);

   if (! strncmp(record, "YES", 3))
      statistics_enabled = TRUE;
   else
      statistics_enabled = FALSE;

   cfg_string(16);
   process_statistics_network_address();

/*
   We added the bad-words look-up file at 04/Sep/90
*/

   cfg_string(17);
   ucase(record);
   STRNCPY(bad_words, record, 20);

/*
   Now get the file attached outbound directory
*/

   cfg_string(18);
   ucase(record);
   STRNCPY(statistics_outbound_directory, record, 65);

   if (statistics_outbound_directory[strlen(statistics_outbound_directory) - 1] != '\\')
      strcat(statistics_outbound_directory, "\\");

/*
   Now get the file attached inbound directory
*/

   cfg_string(19);
   ucase(record);
   STRNCPY(statistics_inbound_directory, record, 65);

   if (statistics_inbound_directory[strlen(statistics_inbound_directory) - 1] != '\\')
      strcat(statistics_inbound_directory, "\\");

/*
   For version 1.61, the following two items were added.

   See if we have a network mail area directory.
*/

   cfg_string(20);
   ucase(record);
   STRNCPY(network_directory, record, 65);

   if (network_directory[strlen(network_directory) - 1] != '\\')
      strcat(network_directory, "\\");

/*
   See if the so-called statistics feature messages should be marked
   as hold for pick up.
*/

   cfg_string(21);
   ucase(record);
   if (!strncmp(record, "YES", 3)) {
      stat_hold = TRUE;
   }
   else {
      stat_hold = FALSE;
   }

/*
    Some systems need to have Mayhem put in an origin line
*/

    cfg_string(22);
    STRNCPY(echo_origin, record, 60);
    echo_origin[strlen(echo_origin) - 1] = (char)NULL; /* Line feed! */

    cfg_string(23);
    STRNCPY(network_origin, record, 60);
    network_origin[strlen(network_origin) - 1] = (char)NULL; /* Line feed! */

/*
   Added around October 1990, turn page of the SysOp off.
*/

   cfg_string(24);
   the_pointer = record;
   page_end_hour = atoi(the_pointer);
   the_pointer += 3;
   page_end_minute = atoi(the_pointer);

   cfg_string(25);
   the_pointer = record;
   page_start_hour = atoi(the_pointer);
   the_pointer += 3;
   page_start_minute = atoi(the_pointer);

/*
   For compatibility reasons, we need to know if the person is running
   Mayhem on a Tandy computer.
*/

   cfg_string(26);
   ucase(record);
   if (! strncmp(record, "YES", 3)) {
      fputs("\n\r- Running on a Tandy Computer -\n\r", stderr);
      tandy = TRUE;
   }

/*
    A new option: see if we should try to reset if Mayhem crashes
*/

   cfg_string(27);
   ucase(record);
   if (! strncmp(record, "YES", 3)) {
       old_interrupt = getvect(0x1c);
       setvect(0x1c, new_clock_tick);
       crash_reset = TRUE;
   }
   else {
       crash_reset = FALSE;
   }

/*
   Are we running over HAM radio?
*/

    cfg_string(28);
    ucase(record);
    if (! strncmp(record, "YES", 3)) {
        ham_version = TRUE;
    }
    else {
        ham_version = FALSE;
    }

/*
    Should we have interrupted serial I/O?
*/

    cfg_string(29);
    ucase(record);
    if (! strncmp(record, "YES", 3)) {
        interrupt_enable = TRUE;
    }
    else {
        interrupt_enable = FALSE;
    }

/*
   Scan for a drop file?

   DORINFO?.DEF and DOOR.SYS are currently supported.
*/

    cfg_string(30);
    ucase(record);
    if (! strncmp(record, "YES", 3)) {
        use_drop_file = TRUE;
    }
    else {
        use_drop_file = FALSE;
    }

/*
    See if we should log chatted conversations
*/

    cfg_string(31);
    ucase(record);
    if (! strncmp(record, "YES", 3)) {
        log_chat = TRUE;
    }
    else {
        log_chat = FALSE;
    }

/*
   End of configuration file. Close it.
*/

   mayhem_fclose(&config, "SHIP.CFG");

/*
   If we are redirected, set up the serial port for interrupts.
*/

   io_init();

/*
   Initialize some values
*/

   srand(time_remaining);
   total_comets = 40;
   the_remove = 0;
   suspended_real_time = FALSE;
   pirate = 0;
   pxpos = pypos = (short)NIL;
   swarm_count = 0;
   swarm_test = 1;
   last_priority = 0;
   decoy_xpos = decoy_ypos = decoy_value = (short)NIL;
   tax_increase = 0.0;
   did_mail = FALSE;
   in_sysop = TRUE;
   time_warning = 0;
   sysop_password_fail = 0;
   highest_message_number = 0;
   drag_ship = (char)NIL;
   entering_mail = FALSE;

/*
   The output resulting from the invocation of this function is only
   sent to the SysOp, never the player, (unless of course the SysOp
   is playing).
*/

   perform_lease_agreement();

   c_out(WHITE, "\n\rUniversal Mayhem (c)\n\r");

   c_out(WHITE,
      "   Copyright 1988/1989/1990/1991 Fredric Rice. Ver: %s %s\n\r",
      THE_VERSION, __DATE__);

   if ((auniverse = mayhem_fopen("UNIVERSE.DAT", "rb", auniverse)) == (FILE *)NULL) {
      c_out(RED, "\n\rUNIVERSE.DAT file is missing. Run: CREATE.EXE to make it.\n\r");
      log_error(20);
      perform_quit(52);
   }

   read_universe(xsize);
   to_save = universe.star;

   if ((aplanets = mayhem_fopen("PLANETS.DAT", "r+b", aplanets)) == (FILE *)NULL) {
      c_out(RED, "\n\rPLANETS.DAT file is missing. Run: CREATE.EXE to make it.");
      log_error(21);
      perform_quit(52);
   }

   if (players > TOTAL_PLAYERS) {
      c_out(RED, "Universal Mayhem only supports up to %d players.\n\r",
          TOTAL_PLAYERS);
      perform_quit(53);
   }

   if ((aship = mayhem_fopen("SHIP.DAT", "r+b", aship)) == (FILE *)NULL) {
      create_ship_file();
   }

   if ((agoals = mayhem_fopen("GOALS.DAT", "r+b", agoals)) == (FILE *)NULL)
      create_goal_file();
   else
      mayhem_fclose(&agoals, "GOALS.DAT");

   plug_ship_values();
   plug_ion_fields();
   plug_goal_data();
   plug_comets_values();

   c_out(WHITE, "      SysOp supports a (%ld by %ld) universe with %d players.\n\r",
      xsize, ysize, players);

   time(&itstime);
   STRNCPY(the_date, ctime(&itstime), 26);
   c_out(WHITE, "         %s\r", the_date);

   c_out(WHITE, "            FidoNet: Zone %d, net %d, node %d, point %d\n\r",
      from_node_zone,
      from_node_network,
      from_node_number,
      from_node_point);

/*
   When can the game be played?
*/

   if (start_hour > 0) {
      the_pointer = the_date;
      the_pointer += 11;
      the_hour = atoi(the_pointer);
      the_pointer += 3;
      the_minute = atoi(the_pointer);

      t_start = (start_hour * 60) + start_minute;
      t_end = (end_hour * 60) + end_minute;
      t_current = (the_hour * 60) + the_minute;

      if (t_current >= t_start && t_current <= t_end) {
         c_out(LIGHTRED, "\n\rGame can't be played now!\n\r");
         c_out(LIGHTRED, "Game playing is not allowed from %02d:%02d to %02d:%02d\n\r",
            start_hour, start_minute, end_hour, end_minute);
         perform_quit(0);
      }
   }

   ship_name[0] = (char)NULL;
   throw_com_port_away();

/* **********************************************************************
   * Set the ships name to all uppercase and take only 4 characters.    *
   * Make sure that "record" only contains characters. This is done by  *
   * calling "check_letters" which will return with the lenth of record *
   * set to null.                                                       *
   *                                                                    *
   ********************************************************************** */

ask_for_ship_name:
   record[0] = (char)NULL;
   count = 0;

   while(strlen(record) != 4) {
      if (count++ == 5) perform_quit(1);
      c_out(WHITE, "\n\rEnter your ships name (4 letters only): ");
      timed_input(0);
      ucase(record);
      check_letters();
   }

   if (! is_it_a_good_name(record)) {
      goto ask_for_ship_name;
   }

   strcpy(ship_name, record);
   user_number = 0;

   for (count = 0; count < players; count++) {
      if (Good_Hold(count)) {
         if (! strcmp(hold[count]->names, ship_name)) {
            user_number = count;
            break;
         }
      }
   }

   if (count == players) {
      already_asked = TRUE;
      make_new_ship();
   }
   else {
       if (use_drop_file) {
           if (! extract_drop_information()) {
               drop_time_remaining = 0;
           }
       }
   }

   read_user();

   if (ships->tax_warnings == (char)AUCTION_SHIP) {
      c_out(RED, "That ship has been put up for auction because of outstanding");
      c_out(RED, " taxes!\n\r");
      goto ask_for_ship_name;
   }

   record[0] = (char)NULL;

/*
   A bug was fixed here. When a ship is created, it used to ask for
   the players password twice. Now we set a flag and only ask if we don't
   know it yet.
*/

   if (! already_asked) {
      for (try_count = 0; try_count < 4; try_count++) {
	 c_out(WHITE, "Enter captains access code: ");
	 timed_input(1);
         ucase(record);
         strcpy(password, record);

         if (strcmp(password, ships->ship_pass)) {
            c_out(RED, "Incorrect password for this ship!\n\r");
            record[0] = (char)NULL;
         }
         else {
            break;
         }
      }
   }

   if (try_count == 4) {
      c_out(RED, "Ask SysOp for help.\n\r");
      perform_quit(1);
   }

   want_color = ships->allow_colors;

/*
   If the ship was destroyed, we ask if the captain wants to start again.
   If so, we will return here to ask for the captains password again. If
   not, we hopefully terminated normally.
*/

   if (hold[user_number]->sxpos == 0 && hold[user_number]->sypos == 0) {
      ship_destroyed();
   }

   if (ships->leashed_to != (char)NIL) {
       if (Good_Hold(ships->leashed_to)) {
           if (hold[ships->leashed_to]->sxpos != 0 && hold[ships->leashed_to]->sypos != 0) {
               c_out(LIGHTRED, "\n\rYour ships' leash to %s is now broken...\n\r",
                   hold[ships->leashed_to]->names);
           }
           else {
               c_out(LIGHTRED, "\n\rThe ship you were leashed to was destroyed!\n\r");
           }
           read_enemy(ships->leashed_to);
           enemy->leashed_by = (char)NIL;
           write_enemy(ships->leashed_to);
       }
   }

   valid_command_file = command_file_exist(user_number);
   get_update_date();
   total_up_slaver_parts();

   c_out(WHITE, "\n\rShips posistion (%ld-%ld) \n\r",
      ships->ship_xpos, ships->ship_ypos);

   plug_scout_information();

   ships->leashed_to = (char)NIL;

/*
   Working in space again. Decriment the morale level just because
*/

   if (ships->ship_morale > 10) {
      ships->ship_morale--;
   }

   write_user();

/*
   Go through the ships that are active and see if we are allied
   with any of them. If so, mark them as friendly, otherwise mark them
   as unfriendly. Notice that we mark ourselves as friendly.

   Also note that if a ship is a friendly, we don't bother to search
   the remaining names in our allies list. This is done by setting
   the try_count to 10 which makes the loop fail.
*/

   for (dloop = 0; dloop < players; dloop++) {
      if (Good_Hold(dloop)) {
	 hold[dloop]->is_friendly = FALSE;
	 for (try_count = 0; try_count < 5; try_count++) {
            if (! strcmp(hold[dloop]->names, ships->allies[try_count])) {
	       hold[dloop]->is_friendly = TRUE;
	       try_count = 10;
	    }
	 }
      }
   }

   hold[user_number]->is_friendly = TRUE;
   log_entry(0);

/* **********************************************************************
   * The ship has not been destroyed so let's put the ships xposistion	*
   * into an easier variable and do the same with the y position.	*
   *									*
   * Do a scan to start out with. Then allow a command to be entered.	*
   * Notice that we do a warp after the scan. We set a flag and then	*
   * perform the warp. This is done so that we can get a status on the	*
   * ships current posistion. If on the base, a planet, etc...		*
   *									*
   * When that is done, we call the plugging of the command files to 	*
   * see how other ships are to react to the players presence.		*
   *									*
   ********************************************************************** */

   xpos = hold[user_number]->sxpos;
   ypos = hold[user_number]->sypos;
   zpos = hold[user_number]->szpos;

   plug_command_files();

   docked = 0;
   infect_count = 0;
   from_remote = FALSE;
   start_warp = 1;

/*
   If today is a Friday, and the statistics feature is enabled, check
   in the mail program to see if we have already composed a statistics
   message package addressed to me.
*/

   if (! strncmp(the_date, "Fri", 3) && statistics_enabled)
      mail_check_statistics_package();

   perform_scan();
   (void)perform_warp();

   if (ships->leashed_by != (char)NIL) {
        if (Good_Hold(ships->leashed_by)) {
            read_enemy(ships->leashed_by);
            enemy->leashed_to = (char)NIL;
            write_enemy(ships->leashed_by);
            c_out(LIGHTRED, "!!! WARNING !!!\n\r");

            c_out(LIGHTGREEN,
                "Ship %s has been leashed to you! Tractor broken!\n\r",
                enemy->ship_name);
        }

        ships->leashed_by = (char)NIL;
        write_user();
   }

   c_out(RED, "Enter HELP for help or HLPn for extended help.\n\r");
   c_out(WHITE, "\n\r");

   rpt_loop = 0;
   start_warp = 0;
   todays_tax_warning = 0;
   time(&time1);
   time(&time_four);
   move_the_cops_around();
   total_pirate_count = 0;
   command_count = 0;

/* **********************************************************************
   *									*
   * When a command string is entered, store it into the back up string	*
   * 'old_record'. Because we want to make the 'A' command work, the	*
   * string should also be copied into 'again_string'. The copy into 	*
   * the again string should only take place when the command is not A.	*
   *									*
   * I'm sorry that a goto/label statement was used here. After some	*
   * consideration, it was determined that setting a flag to bounce out	*
   * of the thing just wasn't worth it.					*
   *									*
   * One of the things added here was the testing to see if the value	*
   * of 'infect_count' is equal to 1. If so, then we need to cause some	*
   * deaths aboard ship.						*
   *									*
   * It is at this point that most of the run-time psudomultitasking	*
   * takes place.							*
   *									*
   * This routine also performs the psudo-real-time operations which	*
   * keep all of the ships with command files active. This is done by	*
   * calling the command file processor BEFORE we actually process the	*
   * command requesteted by the player. One of those processes is the	*
   * warping of the Galactic Police to the area of a ship that is	*
   * either remiss in paying taxes or has fired on a weak ship with 	*
   * phasers.								*
   *									*
   * One of the more interesting ideas that may be tried would be to	*
   * create a queue of processes by storing an array of pointers to	*
   * functions to perform. The actual pointers would really be pointers	*
   * to a structure which also contains a pointer to a function and an	*
   * element of data.							*
   *									*
   * Consider:								*
   *									*
   * A structure which forms a linked list which would contain:		*
   *									*
   *    A pointer to a function to perform				*
   *    An element of data to pass to the function			*
   *    A priority number						*
   *    A pointer to the next structure.				*
   *									*
   * As the distribute_command function gets called whenever the	*
   * active player does anything, we could find the highest priority	*
   * item in the structure, put the data into a variable, then call	*
   * the function, (after removing the item from the linked list).	*
   *									*
   * --->							<---	*
   *									*
   ********************************************************************** */

get_new_command:
   if (ships->taxes > 10000 && todays_tax_warning == 0) {
      c_out(RED, "It's time to pay your taxes!\n\r");
      c_out(WHITE, "%c%c%c%c%c%c%c%c%c%c\n\r", 7, 0, 0, 7, 0, 0, 7, 0, 0, 7);
      todays_tax_warning = 1;
      if (ships->tax_warnings++ == 4) {
	 impound_ship();
         perform_quit(0);
      }
   }

/*
   Choose a random number from 1 to 50. If it's 25, then let there be
   pirates. These pirates will sell plague for 500,000 credits.
*/

   if (arandom(1L, 50L) == 25 && pirate == 0) {
      pirate = 1;
      c_out(LIGHTRED, "\n\rThere are pirates in this area!\n\r");
      pxpos = xpos + arandom(1L, 3L);
      pypos = ypos + arandom(1L, 3L);
      total_pirate_count++;
   }

/*
   If there are pirates, get a random number from 1 to 10 and if its 3,
   mak the pirates leave the area.
*/

   if (pirate == 1) {
      if (arandom(1L, 10L) == 3) {
         pirate = 0;
         pxpos = pypos = (short)NIL;
	 total_pirate_count = 0;
	 c_out(LIGHTRED, "\n\rPirates are warping out of the area!\n\r");
      }
   }

   if (total_pirate_count > 10) {
      pirates_fight_back(FALSE);
      total_pirate_count = 1;
   }

/*
   See if a pirate should slash and run! Ow, this could hurt!
*/

   if (arandom(1L, 100L) == 50 && pirate == 0 && ships->log_count > 10) {
      c_out(LIGHTRED, "* Tracking inbound pirate ship!\n\r");
      pirates_fight_back(FALSE);
      c_out(LIGHTRED, "* Pirate ship is vectoring away at high warp!\n\r");
   }

/*
   Before getting a keyboard/modem input, let's request a real time
   operation. After that, see if the time limit has been reached or
   if a warning should be issued.
*/

   real_time_operation();

   if (swarm_test == 1) {
      if (swarm_sensor_scan(ships->ship_xpos, ships->ship_ypos)) {
         swarm_attack(ships->ship_xpos, ships->ship_ypos, (short) user_number);
      }
   }

/*
   See if 10 minutes have passed since the last cops move. If so,
   move the cops again.
*/

   time(&time_five);
   if (difftime(time_five, time_four) > 600) {
      move_the_cops_around();
      time(&time_four);
   }

   if (is_redirected == 1 && page_flag && command_count > 3) {
      fputs("\n\r---> SYSOP: The player wants to chat! CHAT requested <---\n\r",
          stderr);
      command_count = 0;
   }

   if (rpt_loop == 0) {
      c_out(WHITE, ": ");

      timed_input(0);
      if (total_pirate_count > 0) {
	 total_pirate_count++;
      }
      ucase(record);

      time(&time2);

      game_time_remaining =
         (int) time_remaining - (difftime(time2, time1) / 60);

      if (game_time_remaining == 5 ||
            game_time_remaining == 3 ||
               game_time_remaining == 1) {
         c_out(LIGHTRED, "You have * * * %d minutes * * * left \n\r",
            game_time_remaining);
      }

      if (game_time_remaining < 1) {
         c_out(LIGHTRED, "\n\rGame time limit has been reached.\n\r");
         game_time_remaining = 0;
         perform_quit(0);
      }

/*
   A bug with TBBS was fixed here. People would play up to the time
   allowed, (or nearly so), and when TBBS would reset the computer,
   the playing time would never be updated. Here we set the playing
   time remaining in the users file every time a command is entered.
*/

      ships->time_remaining = game_time_remaining;

/*
   See if their is an infection that should be triggered. If there is,
   call the routine that will do this. After that routine is called,
   acquire a random number from 1 to 10. If it's 5, then set the
   infecttion count back to 10 so it will happen again.
*/

      if (infect_count == 1) {
         trigger_infection();
         the_rnd = arandom(1L, 10L);

         if (the_rnd == 5) {
            infect_count = 10;
         }
      }

      if (infect_count > 0) infect_count--;
      check_after_rpt();
      bail_out = 0;
      strcpy(old_record, record);

      if (! strcmp(record, "A")) {
         strcpy(record, again_string);
	 strcpy(old_record, record);
      }
      else {
         strcpy(again_string, record);
      }
   }

/* **********************************************************************
   * We have a command or a series of commands in 'record'. See if a ;	*
   * character is in the command string. If there is, then more than 1	*
   * command was entered at a time. If this is so, call the routine	*
   * used to strip the command away from the string record. This	*
   * function 'strip_process_command()' will then call the command	*
   * distribution routine.						*
   *									*
   * This variable gets set by the RPT function. This is NEEDED because	*
   * when the operator enters commands AFTER the rptn, those commands	*
   * need to be ignored. For some reason, setting the next byte after	*
   * the RPTn to a NULL didn't make it terminate.			*
   *									*
   ********************************************************************** */

a_loop_here:
   if ((matpoint = match()) != 0) {
      strip_process_command();
      if (bail_out == 0) {
         goto a_loop_here;
      }
      else {
         goto get_new_command;
      }
   }

/* **********************************************************************
   * If only 1 command was entered || this is the last command in the	*
   * command string, we come here. Processing the last command and then	*
   * returning to acquire another command string is performed.		*
   *									*
   ********************************************************************** */

   if (distribute_command()) {
      goto a_loop_here;
   }

   goto get_new_command;
}

/* **********************************************************************
   * A command string has been entered. We want to take the left side 	*
   * of the 'record' string and use it as the command. We also want to	*
   * take the command off of the string and return with the command	*
   * string without the command we executed. When we return, the string *
   * is tested again for additional commands.				*
   *									*
   * To do this, we need to store away the command string, act on the	*
   * command, restore the command string, remove the command, and then	*
   * exit back to the calling routine.					*
   *									*
   ********************************************************************** */

void strip_process_command(void)
{
   char hold_record[201];
   char *opoint;

   strcpy(hold_record, record);
   point = hold_record;
   opoint = point;
   record[matpoint] = (char)NULL;

   if (distribute_command()) {
      return;
   }

   point = opoint;

   for (matpoint++, dloop = 0; dloop < matpoint; dloop++, point++);
   strcpy(record, point);
}

/* **********************************************************************
   * Find ';' within record. Simple enough... Return dloop as the       *
   * character pointer to ';'.                                          *
   *									*
   ********************************************************************** */

short match(void)
{
   char *testing;

   testing = record;

   for (dloop = 0; *testing; dloop++, testing++) {
      if (*testing == ';') {
         return(dloop);
      }
   }
   return(0);
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

int extract_config(short element_number)
{
   char bail_out;

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
      config_bad(element_number);
      log_error(22);
      perform_quit(51);
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

void cfg_string(short element_number)
{
   char bail_out;
   char hold_record[201], *hold_point;

   bail_out = FALSE;

   while (! bail_out) {
      if (! feof(config)) {
         fgets(hold_record, 200, config);
         if (! feof(config)) {
            hold_point = hold_record;
            skipspace(hold_point);
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

   if feof(config) {
      config_bad(element_number);
      log_error(23);
      perform_quit(51);
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
   * The configuration file contained bad information. Either the file  *
   * is not found or the record lengths are a bit unusual, (not         *
   * delimited by a carriage return or something).                      *
   *                                                                    *
   ********************************************************************** */

void config_bad(short element_number)
{
   c_out(LIGHTRED, "\n\rTrouble reading SHIP.CFG file. Read MAYHEM.DOC.");

   if (element_number == 0) {
      c_out(LIGHTRED, "Unable to locate file SHIP.CFG\n\r");
   }
   else {
      c_out(LIGHTRED, " Something wrong with line %d\n\r", element_number);
   }

   if (element_number == 26) {
      c_out(LIGHTRED, "Perhaps the last line doesn't have a carriage ");
      c_out(LIGHTRED, "[RETURN] after allow color value? That could do it.\n\r");
   }
   else {
      c_out(LIGHTRED, "Read MAYHEM.DOC for a sample SHIP.CFG file if you need it.\n\r");
   }
}

/* **********************************************************************
   * If SHIP.DAT is missing, then create the file and stuff it full of  *
   * peoples ships' information. Then return to game play.              *
   *                                                                    *
   ********************************************************************** */

void create_ship_file(void)
{
   c_out(WHITE, "\n\rPutting %d players ship information into SHIP.DAT\n\r", players);

   c_out(WHITE, "Each users file ship record is %d bytes long\n\r",
       sizeof(struct ship_file));

   make_zero_record();

/* **********************************************************************
   * First, create the SHIP.DAT file.                                   *
   *                                                                    *
   * Second, make it writeable binary.                                  *
   *                                                                    *
   ********************************************************************** */

   if ((aship = mayhem_fopen("SHIP.DAT", "wb+", aship)) == (FILE *)NULL) {
      c_out(LIGHTRED, "\n\rUnable to create file: SHIP.DAT\n\r");
      log_error(25);
      perform_quit(52);
   }

/* **********************************************************************
   * Write the data out to the file.                                    *
   * Note that we MUST use a for loop to write out players records.     *
   * This is done, rather than asking for players to be written by the	*
   * fwrite function because it didn't work! The file would be written  *
   * but information from the ENVIRONMENT storage would be written to   *
   * the file. Strange, but true!.                                      *
   *                                                                    *
   ********************************************************************** */

   for (count = 0; count < players; count++) {
      if ((mayhem_fwrite(ships, sizeof(struct ship_file), 1, aship)) != 1) {
	 c_out(LIGHTRED, "\n\rError occured when writing SHIP.DAT record %ld\n\r", count);
	 log_error(26);
         perform_quit(10);
      }
   }

   if ((aship = freopen("SHIP.DAT", "r+b", aship)) == (FILE *)NULL) {
      c_out(LIGHTRED, "\n\rProblems with reopening SHIP.DAT!");
      log_error(27);
      perform_quit(10);
   }

/* **********************************************************************
   * Now that the file has been created and the file reopened, make a	*
   * ship for the Galactic Police.					*
   *									*
   ********************************************************************** */

   create_galactic_police(0);
}

/* **********************************************************************
   * Read in the values of the players ship we want to have memory      *
   * resident. We will read in "players" number of records. If there    *
   * are not enough records in the file, then the file has been         *
   * corrupted some how. If so, ask if the file should be recreated.    *
   *                                                                    *
   ********************************************************************** */

void plug_ship_values(void)
{
   char count;
   for (count = 0; count < players; count++) {
      read_enemy(count);
      *ships = *enemy;

      if (strcmp(ships->ship_name, "NONE")) {
         plug_this_ship(count, TRUE);
         active_players++;
      }
   }
}

/* **********************************************************************
   * This is the function that does the actual memory allocation.	*
   *									*
   ********************************************************************** */

void plug_this_ship(char count, char allocate_memory)
{
   char ploop;
         
   if (allocate_memory) {
       hold[count] = (struct holder *)farmalloc(sizeof(struct holder));

       if (! Good_Hold(count)) {
          c_out(LIGHTRED, "Problem allocating enough memory for ship %d\n\r", count);
          log_error(28);
          perform_quit(10);
       }

       memory_allocated((UL)sizeof(struct holder));
   }

   hold[count]->sxpos = ships->ship_xpos;
   hold[count]->sypos = ships->ship_ypos;
   hold[count]->szpos = ships->ship_universe;
   hold[count]->bxpos = ships->base_xpos;
   hold[count]->bypos = ships->base_ypos;
   hold[count]->bzpos = ships->base_universe;
   hold[count]->standings = ships->planets_owned;
   hold[count]->kills = ships->total_kills;

   for (ploop = 0; ploop < 10; ploop++) {
      if (ships->rem_xpos[ploop] < 0L) {
         ships->rem_xpos[ploop] = 0L;
         ships->rem_ypos[ploop] = 0L;
         ships->rem_universe[ploop] = 0;
      }

      hold[count]->xremotes[ploop] = ships->rem_xpos[ploop];
      hold[count]->yremotes[ploop] = ships->rem_ypos[ploop];
      hold[count]->remote_universe[ploop] = ships->rem_universe[ploop];
   }

   strcpy(hold[count]->names, ships->ship_name);

   for (ploop = 0; ploop < 5; ploop++) {
      hold[count]->xswarm[ploop] = ships->sled_xpos[ploop];
      hold[count]->yswarm[ploop] = ships->sled_ypos[ploop];
      hold[count]->swarm_universe[ploop] = ships->sled_universe[ploop];
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

void ucase(char *this_record)
{
   short i;

   while (*this_record) {
      if (*this_record > 0x60 && *this_record < 0x7b) {
         *this_record = *this_record - 32;
      }
      this_record++;
   }
}

/* **********************************************************************
   * The ship doesn't exist. Ask for password and personal name if the  *
   * player wants to create it.                                         *
   *                                                                    *
   ********************************************************************** */

void make_new_ship(void)
{
   record[0] = (char)NULL;
   make_zero_record();

   while(strlen(record) < 1) {
      c_out(WHITE, "\n\rDo you want to make a new ship named %s? ", ship_name);
      timed_input(0);
      record[1] = (char)NULL;
   }

   ucase(record);
   strcpy(answer, record);

   if (strncmp(answer, "Y", 1)) {
      perform_quit(1);
   }

   strcpy(ships->ship_name, ship_name);

ask_for_password_again:
   record[0] = (char)NULL;

   while(strlen(record) < 4) {
      c_out(WHITE, "Enter the password you want to use (At least 4 characters): ");
      timed_input(0);
      ucase(record);
      record[20] = (char)NULL;
   }

   if (! is_it_a_good_name(record)) {
      goto ask_for_password_again;
   }

   strcpy(ships->ship_pass, record);

   if (use_drop_file) {
       if (extract_drop_information()) {
           strcpy(ships->ship_person, record);
           ships->ship_person[30] = (char)NULL;
           goto skip_name_ask;
       }
       else {
           drop_time_remaining = 0;
       }
   }

ask_for_person_name:
   ships->ship_person[0] = (char)NULL;

   while(strlen(ships->ship_person) < 5) {
      c_out(WHITE, "Enter your name for subspace messages: ");
      timed_input(0);
      strcpy(ships->ship_person, record);
      ships->ship_person[30] = (char)NULL;
   }

skip_name_ask:
   if (! is_it_a_good_name(ships->ship_person)) {
      goto ask_for_person_name;
   }

/*
   If the SysOp allows colors, see if the player wants them.
   Set the character in the players record to reflect that allow or
   disallow depending on what the player wants and what the SysOp want's
*/

   ships->allow_colors = FALSE;

   if (color_enable) {
      c_out(WHITE, "Do you want to have ANSI colors? ");
      timed_input(0);
      ucase(record);
      if (record[0] == 'Y') {
         ships->allow_colors = TRUE;
      }
      else if (record[0] != 'N') {
	 c_out(WHITE, "Defaulting to NO COLORS ALLOWED.\n\r");
      }
   }

   ships->ship_xpos = xsize / 2;
   ships->ship_ypos = ysize / 2;
   ships->ship_universe = 0;
   ships->tax_warnings = 0;

/* **********************************************************************
   * Search through the array and find an empty spot. Write the record  *
   * out to that user_number space and then return. The new ship will   *
   * then be created.                                                   *
   *                                                                    *
   ********************************************************************** */

   for (count = 0; count < players; count++) {
      if (! Good_Hold(count)) {
         c_out(WHITE, "\n\rPlayer number [%ld] activated..\n\r", count);
         user_number = count;
         write_user();
         plug_this_ship(user_number, TRUE);
         return;
      }
   }

   c_out(LIGHTRED, "\n\rThere are already %d players signed up!\n\r", players);
   c_out(LIGHTRED, "Send network mail to SysOp asking for more players to be ");
   c_out(LIGHTRED, "allowed.\n\r");
   perform_quit(53);
}

/* **********************************************************************
   * Check to see if "record" contains alpha characters. Don't allow    *
   * spaces or anything else.                                           *
   *                                                                    *
   * If something is not alpha, set the record length to null so that   *
   * calling routine can check for that condition. Issue a message.     *
   *                                                                    *
   ********************************************************************** */

void check_letters(void)
{
   short count;

   for (count = 0; record[count]; count++) {
      if (! isalpha(record[count])) {
         c_out(WHITE, "\n\rShip name is not all letters. Try again:");
	 record[0] = (char)NULL;
	 return;
      }
   }
   if (! strcmp(record, "NONE")) {
      record[0] = (char)NULL;
   }
}

/* **********************************************************************
   * Display and then update the players last sign-in date and time.    *
   * If they have already signed in today, set the 'time_remaining'	*
   * variable to the remaining time, else leave it alone.		*
   *									*
   * If the last date played to different, copy todays date into the	*
   * ships last date entry and write it to disk.			*
   *									*
   * If we set the time_remaining to zero, then the player has already	*
   * played for the alloted time today.					*
   *                                                                    *
   ********************************************************************** */

void get_update_date(void)
{
   c_out(WHITE, "\n\rLast active: %s", ships->ship_date);

   if (! strncmp(ships->ship_date, the_date, 10)) {
      time_remaining = ships->time_remaining;
   }
   else {
      STRNCPY(ships->ship_date, the_date, 26);
      ships->time_remaining = time_remaining;
      write_user();
   }

   ships->log_count++;

/*
   If we are using a drop file, use the smallest amount of time
*/

   if (use_drop_file && drop_time_remaining != 0) {
      time_remaining = drop_time_remaining;
      if (time_remaining < ships->time_remaining) {
         ships->time_remaining = time_remaining;
      }
      else {
         time_remaining = ships->time_remaining;
      }
   }

   if (time_remaining == 0) {
      c_out(LIGHTRED, "Game playing time has been reached today.\n\r");
      perform_quit(0);
   }
}

/* **********************************************************************
   * by an enemy ship. Let's issue a message and make sure that the	*
   * captain is allowed to have his ship back again.			*
   *									*
   ********************************************************************** */

void ship_destroyed(void)
{
   char who_did_it;

   who_did_it = !strncmp(ships->who_destroyed, "NONE", 4);

   c_out(LIGHTRED,
      "\n\rYour ship has been destroyed by %s\n\r",
      who_did_it ? "an unknown enemy!" : ships->who_destroyed);

   clever_remarks_three();

ask_about_restarting:
   c_out(WHITE, "\n\rDo you want to start all over again? ");
   timed_input(0);
   ucase(record);

   if (record[0] != 'Y' && record[0] != 'N') {
      goto ask_about_restarting;
   }

   if (record[0] == 'N') {
      c_out(LIGHTRED, "\n\rA GOOD captain would hunt down");
      c_out(LIGHTRED, " and destroy %s!\n\r", ships->who_destroyed);
      c_out(LIGHTRED, "Please come back and continue the carnage!\n\r\n\r");
      make_zero_record();
      write_user();
      perform_quit(0);
   }

/*
   Basically, we perform a make zero record, however we want to keep
   the ships name, password, captains name, and anything else we want to
   keep. After we write this information out, we also need to update the
   memory values. We do this by asking the 'plug_this_ship' routine
   to do it.

   Also notice that the base, if any, is not touched. This is so that the
   restarted ship may slowly get to its base where it might be able to
   supply itself.

   IF ANYTHING IS ADDED TO SHIP.H, the player structure, or FILEIO.C,
   then it needs to be changed here as well.
*/

   ships->ship_xpos = xsize / 2;
   ships->ship_ypos = ysize / 2;
   ships->ship_universe = 0;
   ships->ship_power = 50000L;
   ships->ship_shield = 10000;
   ships->ship_credits = 10000;
   ships->ship_cargo = 300;
   ships->ship_crew = 500;
   ships->ship_shuttle = 1;
   ships->ship_hull = 100;
   ships->ship_cloak = 10;
   ships->ship_sensor = 10;
   ships->ship_warp = 10;
   ships->ship_torp = 200;

/*
   Password is _not_ changed!
*/

   for (count = 0; count < 10; count++) {
      ships->rem_xpos[count] = 0L;
      ships->rem_ypos[count] = 0L;
      ships->rem_universe[count] = 0;
   }

   ships->total_kills = 0;
   ships->time_remaining = time_remaining;
   ships->planets_owned = 0;
   ships->plague_flag = 0;
   ships->ship_infected = 0;
   ships->ship_morale = 100;
   ships->taxes = 0.0;
   ships->attack_sleds = 0;

   for (count = 0; count < 5; count++) {
      strcpy(ships->allies[count], "NONE");
   }

   for (count = 0; count < 15; count++) {
      ships->sled_xpos[count] = (short)NIL;
      ships->sled_ypos[count] = (short)NIL;
      ships->sled_swarm[count] = (short)NIL;
      ships->sled_power[count] = 0;
      ships->sled_universe[count] = 0;
   }

   strcpy(ships->last_at_by, "NONE");
   strcpy(ships->last_at_who, "NONE");
   ships->last_at_status = 0;
   ships->log_count = 0;
   strcpy(ships->base_death, "NONE");
   ships->base_hit_count = 0;

/*
   Tag value:

        - Empty spot
        + Planet
        [anything else is a tag name]
*/

   for (count = 0; count < 10; count++) {
      ships->x_planet_info[count] = 0;
      ships->y_planet_info[count] = 0;
      ships->point_tag[count][0] = '-';
   }

   ships->outstanding_bid = (char)NIL;
   ships->bid_amount = 0l;
   strcpy(ships->base_boarded, "NONE");

   for (count = 0; count < 10; count++) {
      ships->planet_taxes[count] = 0.0;
      ships->tax_xpos[count] = (short)NIL;
      ships->tax_ypos[count] = (short)NIL;
      strcpy(ships->slot_owned[count], "NONE");
   }

   for (count = 0; count < OWNABLE; count++) {
      ships->owned_planets[count] = (short)NIL;
   }

   ships->sick_bay = 0;
   ships->bounty = 0L;
   strcpy(ships->who_destroyed, "NONE");

   for (count = 0; count < 10; count++) {
      ships->scout_xpos[count] = (short)NIL;
      ships->scout_ypos[count] = (short)NIL;
      ships->scout_to_x[count] = (short)NIL;
      ships->scout_to_y[count] = (short)NIL;
      ships->scout_direction[count] = (char)NIL;
      ships->scout_universe[count] = 0;
      ships->stay_on_station[count] = TRUE;
   }

/*
   pirate_code is NOT CHANGED!
*/

   ships->leashed_by = (char)NIL;
   ships->leashed_to = (char)NIL;

/*
   Version 2.3 addition
*/

   for (count = 0; count < MACROS; count++)
      ships->macro[count][0] = (char)NULL;

   write_user();
   plug_this_ship(user_number, FALSE);
}

/* **********************************************************************
   * So that the boredum of destroying a planet is alleviated somewhat,	*
   * let's give some range of possible remarks.				*
   *									*
   ********************************************************************** */

void clever_remarks_one(void)
{
   the_rnd = (int) arandom(1L, 15L);
   c_out(GREEN, "\n\r%s\n\r", dead_planet[the_rnd - 1]);
}

/* **********************************************************************
   * When a ship gets destroyed, either by fireing at a cop or running	*
   * into something, or hitting a star too often, display some random	*
   * message.								*
   *									*
   ********************************************************************** */

void clever_remarks_two(void)
{
   the_rnd = (int) arandom(1L, 15L);
   c_out(GREEN, "\n\r%s\n\r", dead_ship[the_rnd - 1]);
}

/* **********************************************************************
   * When a ship gets destroyed and the owner of the ship signes back	*
   * in, we need to say something clever. Because the ship may get	*
   * pulverized many times, we need to randomize the message.		*
   *									*
   ********************************************************************** */

void clever_remarks_three(void)
{
   the_rnd = (int) arandom(1L, 15L);
   c_out(GREEN, "\n\r%s\n\r", restart_ship[the_rnd - 1]);
}

