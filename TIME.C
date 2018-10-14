
/* **********************************************************************
   * time.c								*
   *									*
   * Copyrite 1988, 1989, 1990, 1991,                                   *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   * Simply display the time remaining.					*
   *									*
   ********************************************************************** */

#include "defines.h"
#include "ship.h"
#include "universe.h"
#include "holder.h"
#include "goal.h"
#include "stdio.h"
#include "function.h"
#include "stdlib.h"
#include "time.h"
#include "conio.h"
#include "string.h"

/* **********************************************************************
   * Define the external and local data that this module needs to       *
   * access.                                                            *
   *                                                                    *
   ********************************************************************** */

/* **********************************************************************
   * Define the external and local data that this module needs to       *
   * access.                                                            *
   *                                                                    *
   ********************************************************************** */

   extern char want_color;
   extern char *goal_item_description[10];
   extern FILE *error_log, *ship_log;
   extern long xsize, ysize;
   extern UC zpos;
   extern long aloop, count;
   extern unsigned short ocount;
   extern long ion_field[ION_COUNT][2];
   extern long itstime;
   extern char *record;
   extern char password[21];
   extern char the_date[27];
   extern char *point;
   extern char color_enable;
   extern char is_redirected;
   extern short players;
   extern short user_number;
   extern short time_remaining;
   extern short from_node_zone, from_node_network, from_node_number;
   extern short from_node_point;
   extern short to_node_zone, to_node_network, to_node_number;
   extern short to_node_point;
   extern short stat_node_zone, stat_node_network, stat_node_number;
   extern short stat_node_point;
   extern short game_time_remaining;
   extern short directions[18];
   extern char to_file_listing[20];
   extern char bad_words[21];

/* **********************************************************************
   * Any data this module needs?                                        *
   *                                                                    *
   ********************************************************************** */

   char stat_file_name[100];

/* **********************************************************************
   * Offer the time remaining and times signed in.                      *
   *                                                                    *
   ********************************************************************** */

void perform_time(void)
{
   c_out(WHITE, "Time remaining: %d minutes.\n\r", game_time_remaining);
   c_out(WHITE, "You have signed in %d times.\n\r", ships->log_count);
}

/* **********************************************************************
   * This release notice is copied to some strings, then sent to a	*
   * testing function to see if the checksums match those tested with	*
   * the 'notice_test' function.					*
   *									*
   * If they have been altered in any way that produces an incorrect	*
   * checksum, then we remove the mayhem files, (which won't really	*
   * cause all that much grief because it's still always available or	*
   * on floppy somewhere), and issue a very nasty statement.		*
   *									*
   ********************************************************************** */

void perform_lease_agreement(void)
{
   char r0[60], r1[60], r2[60], r3[60], r4[60], r5[60], r6[60], r7[60];

   strcpy(r0, "*   For the Public Domain. Not to be sold, either by  *\n");
   strcpy(r1, "*   itself, or bundled with other software for ANY    *\n");
   strcpy(r2, "*   cost; at cost, profit or in trade. Distribute     *\n");
   strcpy(r3, "*   freely. 'Companies' which steal from the Public   *\n");
   strcpy(r4, "*   Domain and sells other peoples work are notified  *\n");
   strcpy(r5, "*   that they are NOT granted the authority to sell   *\n");
   strcpy(r6, "*   or distribute my work.                            *\n");
   strcpy(r7, "*               - Fredric L. Rice 1988/1989/1990/1991 *\n");

   notice_test(r0, 4480);
   notice_test(r1, 4616);
   notice_test(r2, 4501);
   notice_test(r3, 4608);
   notice_test(r4, 4851);
   notice_test(r5, 4692);
   notice_test(r6, 3221);
   notice_test(r7, 2992);

   fputs("\n\n", stderr);
   fputs("*******************************************************\n", stderr);
   fputs("* Release agreement:                                  *\n", stderr);
   fputs("*                                                     *\n", stderr);
   fputs(r0, stderr);
   fputs(r1, stderr);
   fputs(r2, stderr);
   fputs(r3, stderr);
   fputs(r4, stderr);
   fputs(r5, stderr);
   fputs(r6, stderr);
   fputs(r7, stderr);
   fputs("*                                                     *\n", stderr);
   fputs("*******************************************************\n", stderr);

   if (is_redirected == 0) {
      c_out(WHITE, "\n\n\n");
   }
}

/* **********************************************************************
   * Take the string being pointed to with 'the_string' and see if the	*
   * checksum matches the value of the test. If not, then issue a	*
   * message and terminate.						*
   *									*
   ********************************************************************** */

void notice_test(char *the_string, short check_sum)
{
   short total_checksum;

   total_checksum = 0;

   while (*the_string) {
      total_checksum += *the_string;
      the_string++;
   }

   if (check_sum == total_checksum) {
      return;
   }

   remove_all_files();
   fputs("\n\n\n\n",                                                  stderr);
   fputs("               YOU HAVE VIOLATED THE RELEASE AGREEMENT!\n", stderr);
   fputs("By fucking with my  release agreement  notice, you have\n", stderr);
   fputs("violated the release  agreement and the articles of the\n", stderr);
   fputs("Copyright underwhich Universal Mayhem was released into\n", stderr);
   fputs("the Public Domain.  If I find out, I will  sue your ass\n", stderr);
   fputs("and it will be mine.\n\n",                                  stderr);
   fputs("If you  didn't  make  the  modifications  to my release\n", stderr);
   fputs("agreement,  then you need to contact me for a good copy\n", stderr);
   fputs("or find one on a system in your area.\n\n",                 stderr);
   abort();
}

/* **********************************************************************
   * Remove all Universal Mayhem files. And one or two others I noticed	*
   * along the way.							*
   *									*
   * No, actually, only remove Univarse Mayhem files; the person may	*
   * have just been playing and there is also the possibility that a	*
   * person may modify the release agreement and upload it for other	*
   * people to get the nasty message.					*
   *									*
   ********************************************************************** */

void remove_all_files(void)
{
   unlink("SHIP.DAT");
   unlink("*.SHP");
   unlink("*.SCT");
   unlink("*.STD");
   unlink("UNIVERSE.DAT");
   unlink("PLANETS.DAT");
   unlink("SHIP.EXE");
   unlink("SHIP.CFG");
   unlink("MAYHEM.EXE");
   unlink("SHIP.HLP");
   unlink("SHIP.INF");
   unlink("GOALS.DAT");
   unlink("COMETS.DAT");
}

/* **********************************************************************
   * An error has occured. Open the error log file for append and add	*
   * the error number to the end of it.					*
   *									*
   * This is basically for the author, to try to find problems.		*
   *									*
   ********************************************************************** */

void log_error(char error_number)
{
   time(&itstime);
   STRNCPY(the_date, ctime(&itstime), 26);

   if ((error_log = mayhem_fopen("SHIP.ERR", "a", error_log)) == (FILE *)NULL) {
      return;
   }

   if (! Good_Hold(user_number)) {
      sprintf(record, "Error %02d occured on %s", error_number, the_date);
   }
   else {
      sprintf(record, "Error %02d occured on %s with ship %s\n\r",
         error_number, the_date, hold[user_number]->names);
   }

   fputs(record, error_log);
   mayhem_fclose(&error_log, "SHIP.ERR");
   c_out(LIGHTRED, "\n\rError %d has been logged\n\r\n\r", error_number);
}

/* **********************************************************************
   * Log something. If the value passed is 0, then log a ship sign in.	*
   * If anything else, log a password attempt on SYSOP.			*
   *									*
   ********************************************************************** */

void log_entry(char log_type)
{
   time(&itstime);
   STRNCPY(the_date, ctime(&itstime), 26);

   if ((ship_log = mayhem_fopen("SHIP.LOG", "a", ship_log)) == (FILE *)NULL) {
      return;
   }

   if (log_type == 0) {
      sprintf(record, "Ship %s signed in at %s",
	 hold[user_number]->names, the_date);
   }
   else {
      sprintf(record, "Player %s tried to access function SYSOP on %s",
         ships->ship_person, the_date);
   }

   fputs(record, ship_log);
   mayhem_fclose(&ship_log, "SHIP.LOG");
}

/* **********************************************************************
   * Allow the change of the password. We must get the current password	*
   * to allow this to be done, of course, to make sure that no one gets	*
   * onto a computer while it's playing and changes it...		*
   *									*
   ********************************************************************** */

void perform_password(void)
{
   char hold_password[23];

   c_out(WHITE, "To change the ships password, you need to tell it to me first.\n\r");
   c_out(WHITE, "\n\r\n\rEnter your ships password: ");
   timed_input(1);
   ucase(record);

   if (strcmp(record, ships->ship_pass)) {
      return;
   }

   c_out(WHITE, "Enter new password: (Up to 20 characters): ");
   timed_input(1);
   ucase(record);
   record[21] = (char)NULL;
   strcpy(hold_password, record);

   if (strlen(record) < 4) {
      c_out(WHITE, "Password needs to be at LEAST 4 characters in length.\n\r");
      return;
   }

   c_out(WHITE, "\n\rEnter new password just to be safe: ");
   timed_input(1);
   ucase(record);

   if (strcmp(record, hold_password)) {
      c_out(LIGHTRED, "New password is different. Change NOT done!\n\r\n\r");
      return;
   }

   read_user();
   strcpy(ships->ship_pass, hold_password);
   write_user();
   c_out(LIGHTBLUE, "New password written. If you forget, ask SysOp to find it!\n\r");
}

/* **********************************************************************
   * If the SysOp allows colors, check to see if the player has colors	*
   * allowed. If so, disallow them, otherwise allow them. Display a	*
   * message depending on what is and is not allowed ot changed.	*
   *									*
   ********************************************************************** */

void perform_color(void)
{
   if (! color_enable) {
      c_out(WHITE, "The SysOp does not have colors for Mayhem enabled.\n\r");
      return;
   }

   read_user();

   if (ships->allow_colors) {
      ships->allow_colors = FALSE;
      want_color = FALSE;
      c_out(WHITE, "Turning colors OFF.\n\r");
   }
   else {
      ships->allow_colors = TRUE;
      want_color = TRUE;
      c_out(WHITE, "Turning colors ON.\n\r");
   }

   write_user();
}

/* **********************************************************************
   * Take the network address, if there is one, and plug the values	*
   * into the variables used. If there is no address, then zero it all.	*
   *									*
   *  1:102/901    or 0                                                 *
   *									*
   ********************************************************************** */

void process_origination_network_address(void)
{
   point = record;
   from_node_zone = find_delimiter(':');
   from_node_network = find_delimiter('/');
   from_node_number = find_delimiter('.');
   from_node_point = find_delimiter(' ');
}

/* **********************************************************************
   * The statistics message should be offered to the network addresses  *
   * that are contained within a file.                                  *
   *                                                                    *
   * Set the various elements to NIL to indicate they are not valid     *
   * and extract the string for the file name.                          *
   *                                                                    *
   ********************************************************************** */

static void process_multi_node_stat_distribution(void)
{
   stat_node_zone = stat_node_network = (short)NIL;
   stat_node_number = stat_node_point = (short)NIL;
   point++;
   skipspace(point);
   STRNCPY(stat_file_name, point, 100);
}

/* **********************************************************************
   * This is the destination network address.                           *
   *                                                                    *
   ********************************************************************** */

void process_destination_network_address(void)
{
   point = record;

   if (*point == '@') {
      to_node_zone = to_node_network = (short)NIL;
      to_node_number = to_node_point = (short)NIL;
      point++;
      STRNCPY(to_file_listing, point, 12);
   }
   else {
      to_node_zone = find_delimiter(':');
      to_node_network = find_delimiter('/');
      to_node_number = find_delimiter('.');
      to_node_point = find_delimiter(' ');
   }
}

/* **********************************************************************
   * This is the statistics network address extraction routine. If the  *
   * string starts with a @ character then it's the name of a file to   *
   * send stat messages to rather than a single address.                *
   *                                                                    *
   ********************************************************************** */

void process_statistics_network_address(void)
{
   point = record;

   if (*point == '@') {
       process_multi_node_stat_distribution();
       return;
   }

   stat_node_zone = find_delimiter(':');
   stat_node_network = find_delimiter('/');
   stat_node_number = find_delimiter('.');
   stat_node_point = find_delimiter(' ');
}

/* **********************************************************************
   * A general function that will look for the passed character in the	*
   * string and return the value that was found after it. Note that the	*
   * remaining delimiters in the string, if any, are used by 'atoi()'	*
   * to find the end of the numeric.					*
   *									*
   ********************************************************************** */

short find_delimiter(char delimit)
{
   short apple;

   apple = atoi(point);

   while (*point) {
      if (*point == delimit) {
	 point++;
	 return(apple);
      }
      point++;
   }
   return(apple);
}

/* **********************************************************************
   * Display the names of the players and if they have bounty on their	*
   * ships->                                                             *
   *									*
   ********************************************************************** */

void perform_bounty(void)
{
   char find_any;

   find_any = FALSE;

   for (count = ocount = 0; count < players; count++) {
      if (Good_Hold(count)) {
         read_enemy(count);
         if (enemy->bounty > 0) {
	    c_out(WHITE,
	       "Ship %s has a bounty of %ld credits on it. It's in Universe {%d}\n\r",
               hold[count]->names, enemy->bounty, hold[count]->szpos);

            find_any = TRUE;

	    if (++ocount == 20) {
	       c_out(WHITE, "\n\rHit [ENTER] to continue: ");
	       timed_input(1);
	       ocount = 0;
	    }
	 }
      }
   }

   for (count = 0; count < players; count++) {
      if (Good_Hold(count)) {
         for (aloop = 0; aloop < 10; aloop++) {
            if (goal_item[aloop] != (struct goal_elements *)NULL) {
               if (goal_item[aloop]->goal_on_ship == (char)count) {
                  c_out(LIGHTRED,
                     "Ship %s has Slaver Weapon part %02d (%s)\n\r",
                     hold[count]->names,
                     (short)aloop,
                     goal_item_description[aloop]);

                  find_any = TRUE;

                  ocount++;
               }
            }
	 }
      }
   }

   if (! find_any)
      c_out(LIGHTGREEN, "No bounty on any ships is reported.\n\r");
}

/* **********************************************************************
   * What is needed here is perhaps four ion fields which are		*
   * contigious in that they are not broken up. Planets and other items	*
   * may be contained within the ion field and should be hidden from	*
   * sensors. Ships as well should be hidden. To this end, the scan	*
   * routine would need to look for ion fields first for each sector.	*
   *									*
   * The problem is one of speed. We are going to need to make the	*
   * scanning for the ion fields VERY fast.				*
   *									*
   * ion_field[ION_COUNT][2] gives the xpos and ypos for up to 100 ion	*
   * field locations.							*
   *									*
   * Notice that here is where we set the random number generation. The	*
   * code is taken directly out of the complete reference book.		*
   *									*
   ********************************************************************** */

void plug_ion_fields(void)
{
   long xion, yion;
   char ion_count;
   short rnd_dir;
   short stime;
   long ltime;

   ltime = time(NULL);
   stime = (unsigned int) ltime / 2;
   srand(stime);

   read_universe(xsize);
   xion = universe.planets[0];
   yion = universe.planets[1];

   for (ion_count = 0; ion_count < ION_COUNT; ion_count++) {
      if (xion > 0 && xion < xsize - 1 && yion > 0 && yion < ysize - 1) {
         ion_field[ion_count][0] = xion;
         ion_field[ion_count][1] = yion;
      }

      rnd_dir = (short)arandom(1L, 9L);
      xion += (short) directions[((short) rnd_dir - 1) * 2L];
      yion += (short) directions[(((short) rnd_dir - 1) * 2L) + 1L];
   }
}

/* **********************************************************************
   * See if the string offered in the test_string is in the file that   *
   * is being pointed to. If so, return TRUE, otherwise return FALSE.   *
   *                                                                    *
   * In either event, rewind the file when an answer is found.          *
   *                                                                    *
   ********************************************************************** */

static char test_this(char *test_string, FILE *bad_file)
{
   char hrecord[201];
   char *c_point;

   if (bad_file == (FILE *)NULL || strlen(test_string) < 3) return(FALSE);;

   ucase(test_string);

   while (! feof(bad_file)) {
      fgets(hrecord, 200, bad_file);
      if (! feof(bad_file)) {
         hrecord[strlen(hrecord) - 1] = (char)NULL;
         ucase(hrecord);
         c_point = hrecord;
         skipspace(c_point);
         if (! strcmp(test_string, c_point)) {
            c_out(LIGHTRED, "!!! Input unacceptable !!! Do it again!\n\r");
            rewind(bad_file);
            return(TRUE);
         }
      }
   }

   rewind(bad_file);
   return(FALSE);
}

/* **********************************************************************
   * Check to see if the string offered matches anything in the 'bad    *
   * words' file and if so, return FALSE, otherwise return TRUE.        *
   *                                                                    *
   * Also, if it's a bad word, say that the input was unacceptable.     *
   *                                                                    *
   ********************************************************************** */

char is_it_a_good_name(char *this_string)
{
   FILE *bad_file;
   char test_string[220], *atpoint;

   if ((bad_file = fopen(bad_words, "rt")) == (FILE *)NULL) return(TRUE);

   atpoint = test_string;
   skipspace(this_string);

   while (*this_string) {
      if (*this_string != ' ') {
          *atpoint++ = *this_string++;
          *atpoint = '\0';
      }
      else {
          skipspace(this_string);
          if (test_this(test_string, bad_file)) {
             fclose(bad_file);
             return(FALSE);
          }
          atpoint = test_string;
      }
   }

   if (test_this(test_string, bad_file)) {
      fclose(bad_file);
      return(FALSE);
   }

   fclose(bad_file);
   return(TRUE);
}

