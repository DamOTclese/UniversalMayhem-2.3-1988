
/* **********************************************************************
   * scout.c                                                            *
   *                                                                    *
   * Copyrite 1990, 1991,                                               *
   * Fredric L. Rice. All rights reserved.                              *
   *                                                                    *
   * Allow the scout commands to work.                                  *
   *                                                                    *
   ********************************************************************** */

#include "defines.h"
#include "stdio.h"
#include "ship.h"
#include "functions.h"
#include "holder.h"
#include "universe.h"
#include "planets.h"
#include "conio.h"
#include "alloc.h"
#include "string.h"
#include "stdlib.h"

/* **********************************************************************
   * Define what the scout structure should look like.                  *
   *                                                                    *
   ********************************************************************** */

   struct scout_info {
      short scout_xpos;         /* Scout ships xposistion               */
      short scout_ypos;         /* Scout ships yposistion               */
      char scout_universe;      /* Scout universe                       */
      char scout_direction;     /* Scout ships inbound/outbound/station */
      short to_xpos;            /* Warping to xposistion                */
      short to_ypos;            /* Warping to yposistion                */
      char flip_flop;           /* Scan only if its 2.                  */
      char stay_on_station;     /* TRUE or FALSE                        */
   } ;

   struct scout_info *scouts[10];

/* **********************************************************************
   * Define the external and local data that this module needs to       *
   * access.                                                            *
   *                                                                    *
   ********************************************************************** */

   extern char is_redirected;
   extern short user_number;
   extern short directions[18];
   extern long xsize, ysize;
   extern long xpos, ypos;
   extern char *record;
   extern unsigned short close_scouts[10];
   extern unsigned short close_ship[TOTAL_PLAYERS];
   extern unsigned short close_base[TOTAL_PLAYERS];
   extern unsigned short close_remotes[TOTAL_PLAYERS];
   extern unsigned short close_swarms[TOTAL_PLAYERS];
   extern char scout_count;
   extern short ship_count;
   extern short base_count;
   extern short remote_count;
   extern short swarm_count;
   extern long dloop;

   static char next_scout_to_move;
   static FILE *scout_file;
   static char scout_file_name[15];

/* **********************************************************************
   * Define the function prototypes for the functions used only here.   *
   *                                                                    *
   ********************************************************************** */

   static char warp_direction(short from_xpos, short from_ypos,
      short to_txpos, short to_typos,
         short *newx, short *newy);

   static void record_planets(char scout_number,
      long x_posistion,
         long y_posistion,
            char technology,
               char visited_by,
                  char protected_by);

   static void sysop_message(char the_direction);
   static void offer_syntax(void);
   static void scout_recall(char scout_number);
   static void scout_place(char scout_number);
   static void scout_destruct(char scout_number);
   static void scout_status(char scout_number);
   static void scout_clear(char scout_number);
   static void record_close_items(char scout_number);
   static void scout_full_report(void);
   static void update_scout_posistion(char the_dir, short newx, short newy);
   static void destruct_on_ship(char on_this);
   static void destruct_on_base(char on_this);
   static void recall_specific(char scount, char was_it_destroyed);
   static void record_ship(char scout_number, char this_on);
   static void record_base(char scout_number, char this_on);
   static void record_remotes(char scout_number, char this_on);
   static void record_swarms(char scout_number, char this_on);
   static char open_scout_for_read(void);

/* **********************************************************************
   * Perform the scout ship operations.                                 *
   *                                                                    *
   * scout{0...9}{command}                                              *
   *                                                                    *
   * o r - Recall. This will pick up the scout ship.                    *
   * o p - Place. This will drop the scout ship and send it on.         *
   * o d - Destruct. This command will destruct the scout ship.         *
   * o s - Status. This command will report on the scout ship.          *
   * o c - Clear. This command will clear the scout ships reports       *
   *                                                                    *
   * scout f {enter}                                                    *
   *                                                                    *
   *    This command above will offer the status of all scout ships     *
   *                                                                    *
   ********************************************************************** */

void perform_scout(void)
{
   char *atpoint, scout_number, the_command;

   atpoint = record;
   atpoint += 5;

   if (*atpoint == (char)NULL) {
      offer_syntax();
      return;
   }

   skipspace(atpoint);

   if (*atpoint == 'F') {
      scout_full_report();
      return;
   }

/*
   Compute the scout ship number
*/

   if (*atpoint >= '0' && *atpoint <= '9') {
      scout_number = *atpoint - '0';
      atpoint++;
   }
   else {
      offer_syntax();
      return;
   }

   skipspace(atpoint);

   switch(*atpoint) {
      case 'R': scout_recall(scout_number); break;
      case 'P': scout_place(scout_number); break;
      case 'D': scout_destruct(scout_number); break;
      case 'S': scout_status(scout_number); break;
      case 'C': scout_clear(scout_number); break;
      default: offer_syntax();
   }
}

/* **********************************************************************
   * Offer the command syntax.                                          *
   *                                                                    *
   ********************************************************************** */

static void offer_syntax(void)
{
   c_out(LIGHTRED, "\n\rSyntax:\n\r   scout{0..9}{command}{enter}   or   ");
   c_out(LIGHTRED, "scoutf{enter}    Commands are:\n\r");
   c_out(LIGHTRED, "   r-recall, p-place, d-destruct, s-status, c-clear\n\r");
}

/* **********************************************************************
   * Go through the ships data record and pull out the scout ship       *
   * information. Allocate enough memory to store that information into *
   * RAM so that its access can be faster than disk. We will also use   *
   * this data structure to automate the scout ships->                   *
   *                                                                    *
   ********************************************************************** */

void plug_scout_information(void)
{
   char sloop;

   next_scout_to_move = 0;

   for (sloop = 0; sloop < 10; sloop++) {
      scouts[sloop] =
         (struct scout_info *)farmalloc((UL)sizeof(struct scout_info));

      if (scouts[sloop] != (struct scout_info *)NULL) {
         memory_allocated((UL)sizeof(struct scout_info));
         scouts[sloop]->scout_xpos = ships->scout_xpos[sloop];
         scouts[sloop]->scout_ypos = ships->scout_ypos[sloop];
         scouts[sloop]->to_xpos = ships->scout_to_x[sloop];
         scouts[sloop]->to_ypos = ships->scout_to_y[sloop];
         scouts[sloop]->scout_direction = ships->scout_direction[sloop];
         scouts[sloop]->scout_universe = ships->scout_universe[sloop];
         scouts[sloop]->flip_flop = 0;
         scouts[sloop]->stay_on_station = ships->stay_on_station[sloop];
      }
   }
}

/* **********************************************************************
   * Automate the move of ONE scout ship.                               *
   *                                                                    *
   * If the ship is inbound                                             *
   *    Determine if the scout ship is in the current universe. If it   *
   *    is not, then don't do anything.                                 *
   *                                                                    *
   *    Compute the direction to travel to move the scout ship from     *
   *    its current posistion to the current posistion of the active    *
   *    ship.                                                           *
   *                                                                    *
   *    If the direction function returns a '5', or a NIL, then the     *
   *    scout ship is not moved.                                        *
   *                                                                    *
   *    If the direction computed is anything other than 5 or NIL,      *
   *    move the scout ship 4 spaces.                                   *
   *                                                                    *
   * If the ship is outbound                                            *
   *    Compute the direction to travel to move the scout ship from     *
   *    its current location to its destination location.               *
   *                                                                    *
   *    If the direction function returns a 5 or NIL, then the scout    *
   *    ship is not moved. If the scout ship is supposed to stay on     *
   *    station, mark it as being on station, otherwise mark it as      *
   *    on its inbound leg.                                             *
   *                                                                    *
   *    If the direction computed is anything other than 5 or NIL, move *
   *    the scout ship 4 spaces.                                        *
   *                                                                    *
   * In any event, after a scout ship moves, check to see if it is the  *
   * second time it has moved without updating the scan records. We     *
   * will only scan and record the surrounding area every other warp.
   *                                                                    *
   ********************************************************************** */

void automate_scout_ships(void)
{
   char the_dir;
   short newx, newy;

   switch(scouts[next_scout_to_move]->scout_direction) {
      case SCOUT_INBOUND:
         if (scouts[next_scout_to_move]->scout_universe ==
                                        hold[user_number]->szpos) {
            the_dir = warp_direction(scouts[next_scout_to_move]->scout_xpos,
               scouts[next_scout_to_move]->scout_ypos,
                  hold[user_number]->sxpos,
                     hold[user_number]->sypos,
                        &newx, &newy);

            update_scout_posistion(the_dir, newx, newy);
         }
         break;

      case SCOUT_OUTBOUND:
         the_dir = warp_direction(scouts[next_scout_to_move]->scout_xpos,
            scouts[next_scout_to_move]->scout_ypos,
               scouts[next_scout_to_move]->to_xpos,
                  scouts[next_scout_to_move]->to_ypos,
                     &newx, &newy);

         update_scout_posistion(the_dir, newx, newy);

         if (the_dir == 5 && scouts[next_scout_to_move]->stay_on_station) {
            scouts[next_scout_to_move]->scout_direction = SCOUT_STATION;
            c_out(LIGHTGREEN,
               "---> Scout ship %d has reached station\n\r",
               next_scout_to_move);
         }
         else if (the_dir == 5) {
            scouts[next_scout_to_move]->scout_direction = SCOUT_INBOUND;
         }
         break;
   }

/*
   See what the next scout ship to move is. If it overflows, write
   the scout ship information to the users data file.
*/

   if (++next_scout_to_move == 10) {
      char sloop;
      read_user();                     
      for (sloop = 0; sloop < 10; sloop++) {
         ships->scout_xpos[sloop] = scouts[sloop]->scout_xpos;
         ships->scout_ypos[sloop] = scouts[sloop]->scout_ypos;
         ships->scout_direction[sloop] = scouts[sloop]->scout_direction;
      }
      write_user();
      next_scout_to_move = 0;
   }
}

/* **********************************************************************
   * Update the scout ships location.                                   *
   *                                                                    *
   ********************************************************************** */

static void update_scout_posistion(char the_dir, short newx, short newy)
{
   if (the_dir != (char)NIL && the_dir != 5) {
      scouts[next_scout_to_move]->scout_xpos = newx;
      scouts[next_scout_to_move]->scout_ypos = newy;
      ships->scout_xpos[next_scout_to_move] = newx;
      ships->scout_ypos[next_scout_to_move] = newy;
      scouts[next_scout_to_move]->flip_flop++;
      if (scouts[next_scout_to_move]->flip_flop == 2) {
         record_close_items(next_scout_to_move);
         scouts[next_scout_to_move]->flip_flop = 0;
      }
   }

   sysop_message(the_dir);
}

/* **********************************************************************
   * Take the offered source location and destination and compute the   *
   * direction. Return the value of the direction after we compute what *
   * the new xposistion and yposistion are.                             *
   *                                                                    *
   * We are provided the address of two variables and these are plugged *
   * with the next xposistion and yposistion.                           *
   *                                                                    *
   ********************************************************************** */

static char warp_direction(short from_xpos, short from_ypos,
   short to_txpos, short to_typos,
      short *newx, short *newy)
{
   char p[4], the_dir, hold_index;

   p[1] = 0;

   if (to_txpos < (from_xpos - 5) && p[1] == 0) {
        p[1] = 1; p[2] = 2; p[3] = 3;
   }

   if (to_txpos > (from_xpos + 5) && p[1] == 0) {
        p[1] = 7; p[2] = 8; p[3] = 9;
   }

   if (p[1] == 0) {
        p[1] = 4; p[2] = 5; p[3] = 6;
   }

   if (to_typos < (from_ypos - 5)) {
        the_dir = p[1];
   }
   else if (to_typos > (from_ypos + 5)) {
        the_dir = p[3];
   }
   else {
        the_dir = p[2];
   }

   hold_index = ((the_dir - 1) * 2);
   from_xpos += 4 * directions[hold_index];
   from_ypos += 4 * directions[++hold_index];

   if (from_xpos < 0 || from_xpos > xsize - 1 ||
       from_ypos < 0 || from_ypos > ysize - 1)
      return((char)NIL);

   *newx = from_xpos;
   *newy = from_ypos;
   return(the_dir);
}

/* **********************************************************************
   * Some SysOps have asked for something more interesting to look at.  *
   * This will display what scout ships are doing and display it only   *
   * to the console, not to the modem. It is only done when the console *
   * has been redirected.                                               *
   *                                                                    *
   ********************************************************************** */

static void sysop_message(char the_direction)
{
   char sysop_output[80];
        
   if (the_direction == (char)NIL) return;

   if (the_direction == 5 &&
      scouts[next_scout_to_move]->scout_direction == SCOUT_STATION) return;

   if (is_redirected == 1) {
      sprintf(sysop_output,
         "SYSOP: Scout %d from ship %s is %s. Heading: %d\n\r",
         next_scout_to_move,
         hold[user_number]->names,
         the_direction == 5 ? "on station" : "warping",
         the_direction);
      fputs(sysop_output, stderr);
   }
}

/* **********************************************************************
   * If the scout is in sensor range, recall it.                        *
   *                                                                    *
   * If the scout is on station, recall it.                             *
   *                                                                    *
   * If the scout is on its outbound leg, inform the player that it is  *
   * not yet reached its destination and is maintaining radio silence.  *
   *                                                                    *
   * If the scout is on its inbound leg, inform the player that it has  *
   * not get got close enough to the ship to be picked up.              *
   *                                                                    *
   ********************************************************************** */

static void scout_recall(char scout_number)
{
   char scount;

   if (scouts[scout_number]->scout_direction == (char)NIL) {
      c_out(LIGHTRED, "That scout is still on board!\n\r");
      return;
   }
   if (scouts[scout_number]->scout_direction == SCOUT_STATION) {
      scouts[scout_number]->scout_direction = SCOUT_INBOUND;
      c_out(LIGHTGREEN,
         "Scout ship has been recalled and is now on its inbound leg.\n\r");
      return;
   }
   if (scouts[scout_number]->scout_direction == SCOUT_DESTROYED) {
      c_out(LIGHTRED, "That scout ship was self-destructed!\n\r");
      return;
   }

   if (scout_count == 0) {
      c_out(LIGHTRED, "There are no scout ships in this area!\n\r");
      return;
   }

   for (scount = 0; scount < scout_count; scount++) {
      if (close_scouts[scount] == scout_number) {
         recall_specific(scount, FALSE);
         c_out(LIGHTGREEN,
            "Scout %d recalled... Request 's' for status.\n\r",
            close_scouts[scount]);
         return;
      }
   }

   c_out(LIGHTRED, "That scout isn't one that's waiting to be picked up\n\r");
   c_out(LIGHTRED, "and it isn't on station yet. Can't recall scout ship!\n\r");
}

/* **********************************************************************
   * Mark the specified scout ship as returned to nothing.              *
   *                                                                    *
   * If the BOOL says TRUE, then mark it as destroyed.                  *
   *                                                                    *
   ********************************************************************** */

static void recall_specific(char scount, char was_it_destroyed)
{
   read_user();
   ships->scout_xpos[close_scouts[scount]] = (short)NIL;
   ships->scout_ypos[close_scouts[scount]] = (short)NIL;
   ships->scout_to_x[close_scouts[scount]] = (short)NIL;
   ships->scout_to_y[close_scouts[scount]] = (short)NIL;
   ships->scout_universe[close_scouts[scount]] = 0;
   ships->stay_on_station[close_scouts[scount]] = TRUE;
   scouts[close_scouts[scount]]->scout_xpos = (short)NIL;
   scouts[close_scouts[scount]]->scout_ypos = (short)NIL;
   scouts[close_scouts[scount]]->to_xpos = (short)NIL;
   scouts[close_scouts[scount]]->to_ypos = (short)NIL;
   scouts[close_scouts[scount]]->scout_universe = 0;
   scouts[close_scouts[scount]]->stay_on_station = TRUE;

   if (! was_it_destroyed) {
      ships->scout_direction[close_scouts[scount]] = (char)NIL;
      scouts[close_scouts[scount]]->scout_direction = (char)NIL;
   }
   else {
      ships->scout_direction[close_scouts[scount]] = SCOUT_DESTROYED;
      scouts[close_scouts[scount]]->scout_direction = SCOUT_DESTROYED;
   }

   write_user();
}

/* **********************************************************************
   * Place the scout ship if not already done so.                       *
   *                                                                    *
   * Ask for the x and y coordinate to go towards                       *
   *                                                                    *
   * Ask if the scout should remain on station once it gets to its      *
   * destination or should turn around and come back to report.         *
   *                                                                    *
   ********************************************************************** */

static void scout_place(char scout_number)
{
   short tox, toy;
   char to_station_or_return;
     
   if (scouts[scout_number]->scout_direction != (char)NIL) {
      c_out(LIGHTRED, "Scout ship %d already deployed!\n\r", scout_number);
      return;
   }

   if (scouts[scout_number]->scout_direction == SCOUT_DESTROYED) {
      c_out(LIGHTRED, "That scout ship was self-destructed!\n\r");
      return;
   }

   c_out(YELLOW, "Send scout ship %d to what X-coordinate? ", scout_number);
   timed_input(0);
   tox = atol(record);
   if (tox < 0 || tox > xsize - 1) {
      c_out(LIGHTRED, "Must be from 0 to %ld!\n\r", xsize - 1);
      return;
   }

   c_out(YELLOW, "Send scout ship %d to what Y-coordinate? ", scout_number);
   timed_input(0);
   toy = atol(record);
   if (toy < 0 || toy > ysize - 1) {
      c_out(LIGHTRED, "Must be from 0 to %ld!\n\r", ysize - 1);
      return;
   }

   c_out(YELLOW, "Stay on station or return: (S) or (R): ");
   timed_input(0);
   ucase(record);
   if (record[0] != 'S' && record[0] != 'R') {
      c_out(LIGHTRED, "Scout deployment terminated!\n\r");
      return;
   }

   read_user();
   ships->scout_xpos[scout_number] = hold[user_number]->sxpos;
   ships->scout_ypos[scout_number] = hold[user_number]->sypos;
   ships->scout_universe[scout_number] = hold[user_number]->szpos;
   ships->scout_to_x[scout_number] = tox;
   ships->scout_to_y[scout_number] = toy;
   ships->scout_direction[scout_number] = SCOUT_OUTBOUND;
   ships->stay_on_station[scout_number] = (record[0] == 'S');

   scouts[scout_number]->scout_xpos = hold[user_number]->sxpos;
   scouts[scout_number]->scout_ypos = hold[user_number]->sypos;
   scouts[scout_number]->scout_universe = hold[user_number]->szpos;
   scouts[scout_number]->to_xpos = tox;
   scouts[scout_number]->to_ypos = toy;
   scouts[scout_number]->scout_direction = SCOUT_OUTBOUND;
   scouts[scout_number]->stay_on_station = (record[0] == 'S');
   write_user();

   c_out(LIGHTGREEN,
      "Scout %d deployed... On outbound leg of journey.\n\r", scout_number);
}

/* **********************************************************************
   * Make sure that the destruct is actually wanted. If it is, then     *
   * have it destruct and check the area around the scout to see if     *
   * some damage should be inflicted on enemy ships and things in the   *
   * general area.                                                      *
   *                                                                    *
   ********************************************************************** */

static void scout_destruct(char scout_number)
{
   char got_one;
   char aloop;
      
   if (scouts[scout_number]->scout_direction == SCOUT_DESTROYED) {
      c_out(LIGHTRED, "That scout ship was already self-destructed!\n\r");
      return;
   }

   if (scouts[scout_number]->scout_direction == (char)NIL) {
      c_out(LIGHTRED,
         "Scout ship %d is on board! Destruct sequence aborted.\n\r",
         scout_number);
      return;
   }

/*
   See what's in the area
*/

   plug_close_objects(CLOSE_NO_ION);
   got_one = FALSE;

   for (aloop = 0; aloop < ship_count; aloop++) {
      if (close_ship[aloop] != user_number) {
         destruct_on_ship(aloop);
         got_one = TRUE;
      }
   }

   for (aloop = 0; aloop < base_count; aloop++) {
      if (close_base[aloop] != user_number) {
         destruct_on_base(aloop);
         got_one = TRUE;
      }
   }

   if (! got_one) {
      c_out(LIGHTRED, "There are no enemy ships, or bases in this\n\r");
      c_out(LIGHTRED, "area! Scout destruction is not allowed!\n\r");
      return;
   }
   else {
      recall_specific(scout_number, TRUE);

      c_out(LIGHTRED,
         "\n\rGertzits! Ker-Blamppp! - Scout ship %d totally destroyed!\n\r\n\r",
         scout_number);
   }
}

/* **********************************************************************
   * Cause some damage to the enemy ship pointed to by the value passed *
   * to the function. Check to see if the ship was allied to the active *
   * ship and if it was, break the alliance.                            *
   *                                                                    *
   ********************************************************************** */

static void destruct_on_ship(char on_this)
{
   long fpercent;
   long crew_started;
         
   read_enemy(close_ship[on_this]);
   fpercent = (int)arandom(30L, 40L);

   c_out(LIGHTRED,
      "Enemy ship [%s] heavily damaged by explosion, %d losses:\n\r",
      hold[close_ship[on_this]]->names,
      (short)fpercent);

   crew_started = enemy->ship_crew;
   enemy->ship_power  -= (enemy->ship_power  * (fpercent / 100));
   enemy->ship_shield -= (enemy->ship_shield * (fpercent / 100));
   enemy->ship_crew   -= (enemy->ship_crew   * (fpercent / 100));
   enemy->ship_hull   -= (enemy->ship_hull   * (fpercent / 100));
   enemy->sick_bay += arandom(1L, crew_started);

   c_out(LIGHTBLUE, "   Enemy power down to %ld units\n\r",
      enemy->ship_power);

   c_out(LIGHTBLUE, "   Enemy shields down to %ld units\n\r",
      enemy->ship_shield);

   c_out(LIGHTBLUE, "   Enemy crew casualties: %d, %d now to sick bay\n\r",
      crew_started - enemy->ship_crew, enemy->sick_bay);

   c_out(LIGHTBLUE, "   Enemy ships hull down to %d\n\r",
      enemy->ship_hull);

   strcpy(enemy->last_at_by, hold[user_number]->names);
   write_enemy(close_ship[on_this]);
}

/* **********************************************************************
   * Cause some damage to the enemy base pointed to by the value passed *
   * to the function. Check to see if the base was allied to the active *
   * ship and if it was, break the alliance.                            *
   *                                                                    *
   ********************************************************************** */

static void destruct_on_base(char on_this)
{
   long fpercent;
   long crew_killed;
         
   read_enemy(close_base[on_this]);
   fpercent = (int)arandom(30L, 40L);

   c_out(LIGHTRED,
      "Enemy base [%s] heavily damaged by explosion, %d losses:\n\r",
      hold[close_base[on_this]]->names,
      (short)fpercent);

   crew_killed = (enemy->base_crew * (fpercent / 100));
   enemy->base_shield -= (enemy->base_shield * (fpercent / 100));
   enemy->base_crew   -= crew_killed;

   c_out(LIGHTBLUE, "   Enemy base shields down to %ld units\n\r",
      enemy->base_shield);

   c_out(LIGHTBLUE, "   Enemy base crew casualties: %d killed!\n\r",
      crew_killed);

   strcpy(enemy->last_at_by, hold[user_number]->names);
   write_enemy(close_base[on_this]);
}

/* **********************************************************************
   * Get the status report of the requested scout ship. Give the        *
   * location and status of the scout ship before offering the data in  *
   * the scout ships data file.                                         *
   *                                                                    *
   ********************************************************************** */

static void scout_status(char scout_number)
{
   char *atpoint;
   char record[201];
     
   if (scout_number < 0 || scout_number > 9) {
      offer_syntax();
      return;
   }

   c_out(LIGHTGREEN, "--- Status of scout ship %d ---\n\r", scout_number);

   if (! open_scout_for_read()) {
      c_out(LIGHTRED,
      "Unable to obtain scout ship records. There may be none.\n\r");
      return;
   }
        
   while (! feof(scout_file)) {
      fgets(record, 200, scout_file);
      if (! feof(scout_file)) {
         if (record[0] == '0' + scout_number) {
            atpoint = record;
            atpoint++;
            c_out(LIGHTBLUE, "%s\r", atpoint);
         }
      }
   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Clear accumulated information from the offer scout ships data      *
   * file.                                                              *
   *                                                                    *
   ********************************************************************** */

static void scout_clear(char scout_number)
{
   char found_how_many;
   FILE *temp_file;
   char *atpoint;
   char record[201];
   char test_byte;

   if (scout_number < 0 || scout_number > 9) {
      offer_syntax();
      return;
   }

   if (! open_scout_for_read()) {
      c_out(LIGHTRED, "Unable to find any scout ship records to clear.\n\r");
      return;
   }

   found_how_many = 0;

   temp_file = (FILE *)NULL;
   temp_file = mayhem_fopen("SHIP.TMP", "wt", temp_file);
   if (temp_file == (FILE *)NULL) {
      log_error(132);
      return;
   }
        
   while (! feof(scout_file)) {
      fgets(record, 200, scout_file);
      if (! feof(scout_file)) {
         if (record[0] != '0' + scout_number) {
            test_byte = fputs(record, temp_file);
            if (test_byte == (char)EOF) {
               log_error(133);
               mayhem_fclose(&scout_file, scout_file_name);
               mayhem_fclose(&temp_file, "SHIP.TMP");
               return;
            }
         }
         else {
            found_how_many++;
         }
      }
   }

   mayhem_fclose(&scout_file, scout_file_name);
   mayhem_fclose(&temp_file, "SHIP.TMP");

   if (found_how_many > 0) {
      sprintf(scout_file_name, "%s.SCT", hold[user_number]->names);
      if (unlink(scout_file_name) != 0) {
         c_out(LIGHTRED, "Unable to update scout ship file!\n\r");
         log_error(50);
         return;
      }
      if (rename("SHIP.TMP", scout_file_name) != 0) {
         c_out(LIGHTRED, "Problem renaming your scout ship file!\n\r");
         log_error(51);
         return;
      }
      c_out(LIGHTGREEN,
      "--- Cleared status information for scout ship %d ---\n\r", scout_number);
   }
   else {
      c_out(LIGHTGREEN, "--- No records were located ---\n\r");
      return;
   }
}

/* **********************************************************************
   * See if there is anything of interest to record around the scout    *
   * ship that is currently active.                                     *
   *                                                                    *
   * The location and technology level of planets they come close to    *
   * The location and "ping" level of ships and bases they come close 2 *
   * The location of remote robot sensors they come close to            *
   * The location and strengths of attack sled swarms they come close 2 *
   *                                                                    *
   * store xpos and ypos                                                *
   * replace them with xpos and ypos of the scout ship                  *
   * plug close objects                                                 *
   * check for planets                                                  *
   * extract and record the information                                 *
   * restore the original xpos and ypos                                 *
   * plug close objects                                                 *
   *                                                                    *
   ********************************************************************** */

static void record_close_items(char scout_number)
{
   long hold_x, hold_y;
   char aloop;
   short x_walk;        /* Walk through planet scan     */
   char p_walk;         /* Check all four planets       */

   hold_x = xpos;
   hold_y = ypos;
   xpos = scouts[scout_number]->scout_xpos;
   ypos = scouts[scout_number]->scout_ypos;
   plug_close_objects(CLOSE_NO_ION);

   for (aloop = 0; aloop < ship_count; aloop++) {
      if (user_number != close_ship[aloop]) {
         read_enemy(close_ship[aloop]);
         record_ship(scout_number, close_ship[aloop]);
      }
   }

   for (aloop = 0; aloop < base_count; aloop++) {
      if (user_number != close_base[aloop]) {
         read_enemy(close_base[aloop]);
         record_base(scout_number, close_base[aloop]);
      }
   }

   for (aloop = 0; aloop < remote_count; aloop++) {
      if (user_number != close_remotes[aloop]) {
         record_remotes(scout_number, close_remotes[aloop]);
      }
   }

   for (aloop = 0; aloop < swarm_count; aloop++) {
      if (user_number != close_swarms[aloop]) {
         record_swarms(scout_number, close_swarms[aloop]);
      }
   }

   for (x_walk = xpos - 5; x_walk < xpos + 5; x_walk++) {
      if (x_walk > 1 && x_walk < xsize - 1) {
         read_universe(x_walk);
         for (p_walk = 0; p_walk < 4; p_walk++) {
            if (universe.planets[p_walk] > ypos - 8 &&
                universe.planets[p_walk] < ypos + 8) {
               dloop = p_walk;
               read_planets(x_walk);
               record_planets(scout_number,
                  x_walk,
                  universe.planets[p_walk],
                  planets.technology,
                  planets.visited,
                  planets.protected);
            }
         }
      }
   }

   xpos = hold_x;
   ypos = hold_y;
   plug_close_objects(CLOSE_NORMAL);
}

/* **********************************************************************
   * Open the scout file for append and return the result of the open.	*
   *									*
   ********************************************************************** */

static char open_scout_file_for_append(void)
{
   sprintf(scout_file_name, "%s.SCT", hold[user_number]->names);
   scout_file = (FILE *)NULL;
   scout_file = mayhem_fopen(scout_file_name, "at", scout_file);
   return(scout_file != (FILE *)NULL);
}

/* **********************************************************************
   * Open the scout file for read and write in text mode. Return a TRUE *
   * if the pointer returned is not NULL.                               *
   *                                                                    *
   ********************************************************************** */

static char open_scout_for_read(void)
{
   sprintf(scout_file_name, "%s.SCT", hold[user_number]->names);
   scout_file = (FILE *)NULL;
   scout_file = mayhem_fopen(scout_file_name, "rt", scout_file);
   return(scout_file != (FILE *)NULL);
}

/* **********************************************************************
   * Open the ships scout file for append and store enemy ships info.   *
   *                                                                    *
   ********************************************************************** */

static void record_ship(char scout_number, char this_on)
{
   char out_buffer[80];
   char test_byte;
        
   if (! open_scout_file_for_append()) return;

   sprintf(out_buffer, "%dEnemy ship [%s] at {%ld-%ld}, ping value %d\n",
      scout_number,
      hold[this_on]->names,
      hold[this_on]->sxpos,
      hold[this_on]->sypos,
      (short)enemy->ship_power / 1000000L);

   test_byte = fputs(out_buffer, scout_file);

   if (test_byte == (char)EOF) {
      log_error(134);
   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Open the ships scout file for append and store enemy bases info.   *
   *                                                                    *
   ********************************************************************** */

static void record_base(char scout_number, char this_on)
{
   char out_buffer[80];
   char test_byte;

   if (! open_scout_file_for_append()) return;

   sprintf(out_buffer, "%dEnemy base [%s] at {%ld-%ld}, ping value %d\n",
      scout_number,
      hold[this_on]->names,
      hold[this_on]->sxpos,
      hold[this_on]->sypos,
      (short)enemy->base_shield / 1000000L);

   test_byte = fputs(out_buffer, scout_file);

   if (test_byte == (char)EOF) {
      log_error(135);
   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Open the ships scout file for append and store enemy remotes info. *
   *                                                                    *
   ********************************************************************** */

static void record_remotes(char scout_number, char this_on)
{
   char out_buffer[80];
   char test_byte;

   if (! open_scout_file_for_append()) return;

   sprintf(out_buffer, "%dEnemy remote [%s] around {%ld-%ld}\n",
      scout_number,
      hold[this_on]->names,
      xpos,
      ypos);

   test_byte = fputs(out_buffer, scout_file);

   if (test_byte == (char)EOF) {
      log_error(136);
   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Open the ships scout file for append and store enemy swarms info.  *
   *                                                                    *
   ********************************************************************** */

static void record_swarms(char scout_number, char this_on)
{
   char out_buffer[80];
   char test_byte;

   if (! open_scout_file_for_append()) return;

   sprintf(out_buffer, "%dEnemy attack sleds [%s] around {%ld-%ld}\n",
      scout_number,
      hold[this_on]->names,
      xpos,
      ypos);

   test_byte = fputs(out_buffer, scout_file);

   if (test_byte == (char)EOF) {
      log_error(137);
   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Open the ships scout file for append and store planets information *
   *                                                                    *
   ********************************************************************** */

static void record_planets(char scout_number,
   long x_posistion, long y_posistion,
   char technology, char visited_by,
   char protected_by)
{
   char out_buffer[80];
   char test_byte;

   if (! open_scout_file_for_append()) return;

   sprintf(out_buffer, "%dPlanet at {%ld-%ld} technology %d\n",
      scout_number, x_posistion, y_posistion, technology);

   test_byte = fputs(out_buffer, scout_file);

   if (test_byte == (char)EOF) {
      log_error(138);
      mayhem_fclose(&scout_file, scout_file_name);
      return;
   }

   if (visited_by != (char)NIL &&
              Good_Hold(visited_by) &&
                        strcmp("NONE", hold[visited_by]->names)) {
      sprintf(out_buffer, "%d   Last visited by %s\n",
         scout_number, hold[visited_by]->names);

      test_byte = fputs(out_buffer, scout_file);
      if (test_byte == (char)EOF) {
         log_error(139);
         mayhem_fclose(&scout_file, scout_file_name);
         return;
      }
   }

   if (protected_by != (char)NIL &&
               Good_Hold(protected_by) &&
                       strcmp("NONE", hold[protected_by]->names)) {
      sprintf(out_buffer, "%d   Protected by %s\n",
         scout_number, hold[protected_by]->names);

      test_byte = fputs(out_buffer, scout_file);
      if (test_byte == (char)EOF) {
         log_error(140);
         mayhem_fclose(&scout_file, scout_file_name);
         return;
      }

   }

   mayhem_fclose(&scout_file, scout_file_name);
}

/* **********************************************************************
   * Offer a full report on all scout ships->                            *
   *                                                                    *
   ********************************************************************** */

static void scout_full_report(void)
{
   char sloop, cloop;

   scout_count = 0;
   plug_close_scouts();

   for (sloop = 0; sloop < 10; sloop++) {
      c_out(LIGHTBLUE, "Scout %d - %c ", sloop,
         sloop == next_scout_to_move ? '*' : ' ');
      for (cloop = 0; cloop < scout_count; cloop++) {
         if (close_scouts[cloop] == sloop) {
            c_out(LIGHTRED, "[in sensor range]\n\r            ");
         }
      }
      switch(scouts[sloop]->scout_direction) {
         case SCOUT_INBOUND:
         case SCOUT_OUTBOUND:
            c_out(LIGHTGREEN, "%s, {%d - %d} Universe %d %s {%d - %d}, will %s\n\r",
               scouts[sloop]->scout_direction == SCOUT_INBOUND ? "inbound" : "outbound",
               scouts[sloop]->scout_xpos,
               scouts[sloop]->scout_ypos,
               scouts[sloop]->scout_universe,
               scouts[sloop]->scout_direction == SCOUT_INBOUND ? "from" : "to",
               scouts[sloop]->to_xpos,
               scouts[sloop]->to_ypos,
               scouts[sloop]->stay_on_station ? "stay" : "return");
            break;
         case SCOUT_STATION:
            c_out(LIGHTGREEN, "holding station at {%d - %d}\n\r",
               scouts[sloop]->scout_xpos, scouts[sloop]->scout_ypos);
            break;
         case SCOUT_DESTROYED:
            c_out(LIGHTRED, "self-destructed at {%d - %d} universe %d\n\r",
               scouts[sloop]->scout_xpos,
               scouts[sloop]->scout_ypos,
               scouts[sloop]->scout_universe);
            break;
         default:
            c_out(LIGHTGREEN, "on board\n\r");
            break;
      }
   }
}


