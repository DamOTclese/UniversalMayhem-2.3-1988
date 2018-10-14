
/* **********************************************************************
   * mail.c								*
   *									*
   * Copyrite 1988, 1989, 1990, 1991.                                   *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   ********************************************************************** */

#include "defines.h"
#include "holder.h"
#include "ship.h"
#include "stdio.h"
#include "function.h"
#include "dir.h"
#include "conio.h"
#include "string.h"
#include "stdlib.h"

/* **********************************************************************
   * The string array 'allow_mail' determines if mail is enabled by	*
   * the system operator. Some BBS packages do not allow proper		*
   * subspace mail handleing.						*
   *									*
   ********************************************************************** */

   extern char allow_mail[4];
   extern FILE *mail, *msg_file, *subspace_messages;
   extern FILE *command_file;
   extern long count;
   extern char *record;
   extern char *subspace_mail;
   extern short players;
   extern short active_players;
   extern short user_number;
   extern short from_node_zone, from_node_network, from_node_number;
   extern short from_node_point, to_node_zone, to_node_network;
   extern short to_node_number, to_node_point;
   extern short stat_node_zone, stat_node_network, stat_node_number;
   extern short stat_node_point;
   extern char the_date[27];
   extern char statistics_enabled;
   extern char is_redirected;
   extern long xsize, ysize;
   extern char want_color;
   extern char color_enable;
   extern char *statistics_outbound_directory;
   extern char *network_directory;
   extern char stat_hold;
   extern char *echo_origin;
   extern char *network_origin;
   extern char files_opened;
   extern char crash_reset;
   extern char entering_mail;
   extern char *continue_last;
   extern char *goal_item_description[10];
   extern char stat_file_name[100];
   extern char *point;

/* **********************************************************************
   * If we have revectored the 0x1c interrupt, set it back.             *
   *                                                                    *
   ********************************************************************** */

   extern void interrupt (*old_interrupt)(void);

/* **********************************************************************
   * An interesting update requested by Astro-Nets SysOp: The did_mail	*
   * flag determines what error level to return to the operating	*
   * system in the event of a normal exit. This is so that the batch	*
   * file handeling the Universal Mayhem package may process the mail	*
   * slot only if main had been entered and not waste time if it had	*
   * not been entered.							*
   *									*
   ********************************************************************** */

   extern char did_mail;

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

   struct ffblk file_block;
   char to_file_listing[20];
   static FILE *mail_list;
   static int original_message_number;
   static char hold_message_file_name[81];
   static char mbuffer[40];

/* **********************************************************************
   * Here are the function prototypes that are used only here.          *
   *                                                                    *
   ********************************************************************** */

   static void copy_mail_to_all_nodes(void);
   static short find_highest_message_number(char *directory_search);
   void create_stat_package(void);

/* **********************************************************************
   *									*
   * Create a subspace message in OPUS format. Allow any number of	*
   * lines to be put in, (up to 2K), and terminate on blank line.	*
   *									*
   * This routine will be replaced with a wrap-around message entry 	*
   * which I think can be taken from a program called 'bbs.c'.		*
   *									*
   ********************************************************************** */

void perform_msg(void)
{
   char arecord[201], *atpoint;

   if (strncmp(allow_mail, "ON", 2)) {
      c_out(WHITE, "Subspace mail can't be sent due to heavy noise levels\n\r");
      return;
   }

   strcpy(arecord, record);
   atpoint = arecord;
   atpoint += 3;

   if (strlen(atpoint) > 4) {
      while (*atpoint == 0x20 && *atpoint) {
         atpoint++;
      }
   }

   if (strlen(atpoint) == 4) {
      strcpy(record, atpoint);
      ucase(record);
   }
   else {
      c_out(WHITE, "Enter the name of the ship to send message to (Or ALL): ");
      timed_input(0);
      ucase(record);
      record[4] = (char)NULL;
   }

   if (! strcmp(record, "ALL")) {
      message_to_this((short)NIL, TRUE, "");
      return;
   }

   if (strlen(record) != 4) {
      return;
   }

   if (! strcmp(record, hold[user_number]->names)) {
      c_out(WHITE, "Send mail to yourself? Don't be silly!\n\r");
      return;
   }

   for (count = 0; count < players; count++) {
      if (Good_Hold(count)) {
         if (! strcmp(record, hold[count]->names)) {
            message_to_this(count, TRUE, "");
            return;
         }
      }
   }

   message_to_this(count, FALSE, record);
}

/* **********************************************************************
   * The message name is valid. If the count number is -1 then it's a	*
   * message to all.							*
   *									*
   * If the 'all_or_good_name' is TRUE, then the user entered all or    *
   * a name that was found. If it's TRUE, we address the message to     *
   * either all players or to the specified ship. If it's FALSE, then   *
   * we use the offered name and address it as a remote player.         *
   *                                                                    *
   ********************************************************************** */

void message_to_this(short count, char all_or_good_name, char *use_this)
{
   char *atpoint;
   FILE *read_in;
   char ship_name[5];
   char test_byte;

   if (! all_or_good_name)
      strcpy(ship_name, use_this);
         
ask_for_subject:
   c_out(WHITE, "Enter subject: ");
   timed_input(0);

   if (strlen(record) < 2)
      strcpy(record, "Subspace message");

   record[35] = 0;

   if (! is_it_a_good_name(record)) {
      goto ask_for_subject;
   }

   if (! make_new_message(FALSE)) return;

/*
   Put in the origin name
*/

   strcpy(message.from, hold[user_number]->names);

/*
   Put in the destination name
*/

   if (count == (short)NIL) {
      strcpy(message.to, "All Universal Mayhem Players");
   }
   else if (all_or_good_name) {
      strcpy(message.to, hold[count]->names);
   }
   else {
      strcpy(message.to, ship_name);
      strcat(message.to, ", remote player");
      c_out(LIGHTRED, "Message to: %s\n\r", message.to);
   }

/*
   Put in the subject
*/

   strcpy(message.subject, record);

/*
   Format and put the current date and time in
*/

   message.date[0] = the_date[8];
   message.date[1] = the_date[9];
   message.date[2] = ' ';
   message.date[3] = the_date[4];
   message.date[4] = the_date[5];
   message.date[5] = the_date[6];
   message.date[6] = ' ';
   message.date[7] = the_date[22];
   message.date[8] = the_date[23];
   message.date[9] = ' ';
   message.date[10] = ' ';
   message.date[11] = (char)NULL;
   atpoint = the_date;
   atpoint += 11;
   strncat(message.date, atpoint, 8);

   /*    '19 Aug 60  19:49:35'    */

/*
   Put the data in
*/

   message.destination_node = to_node_number;
   message.originate_node = from_node_number;
   message.cost = 0;
   message.originate_net = from_node_network;
   message.destination_net = to_node_network;
   message.destination_zone = to_node_zone;
   message.originate_zone = from_node_zone;
   message.destination_point = to_node_point;
   message.originate_point = from_node_point;
   message.reply = 0;
   message.attribute = 2 + 256; /* Crash Local */
   message.upwards_reply = 0;
   message.a_character_1 = 1;
   strcpy(message.area_name, "AREA:MAYHEM");

   if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
      c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
      c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
      c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
      c_out(LIGHTRED, "Ship-to-ship message not sent!\n\r");
      mayhem_fclose(&msg_file, hold_message_file_name);
      log_error(120);
      return;
   }

   msg_out(13);
   msg_out(10);

   msg_out(1);

   strcpy(record,
      "Universal Mayhem (C) Fredric L. Rice (1:102/901) Subspace Mail");

   plug_msg(record, strlen(record));
   msg_out(13);
   msg_out(10);

   c_out(WHITE, "\n\rEnter subspace message. To end, hit enter on a blank line.\n\r");

continue_message_entry:
   entering_mail = TRUE;

   while (strlen(record) > 0) {
      c_out(WHITE, "\n\rMail> ");
      timed_input(0);

      if (! is_it_a_good_name(record)) {
         goto continue_message_entry;
      }

      plug_msg(record, strlen(record));
      msg_out(13);
      msg_out(10);
   }

ask_for_the_option:
   entering_mail = FALSE;
   c_out(WHITE, "\n\r(C)ontinue, (S)ave, (A)bort: ");
   timed_input(0);
   ucase(record);
   record[1] = (char)NULL;

   if (record[0] == 'C') {
      goto continue_message_entry;
   }
   else if (record[0] == 'R' && is_redirected == 0) {
      c_out(LIGHTRED, "Enter file name to read into message: ");
      timed_input(0);
      if (strlen(record) < 1) {
         c_out(LIGHTRED, "Read-in aborted...\n\r");
         goto ask_for_the_option;
      }
      if ((read_in = fopen(record, "rt")) == (FILE *)NULL) {
         c_out(LIGHTRED, "File: %s wasn't found!\n\r", record);
         goto ask_for_the_option;
      }
      while (! feof(read_in)) {
         fgets(record, 200, read_in);
         if (! feof(read_in)) {
            c_out(LIGHTGREEN, "%s\r", record);
            plug_msg(record, strlen(record));
         }
      }
      test_byte = fclose(read_in);
      if (test_byte != 0) {
         log_error(123);
      }
      goto ask_for_the_option;
   }
   else if (record[0] == 'S') {
      sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
      plug_msg(record, strlen(record));
      strcpy(record, echo_origin);
      ucase(record);

      if (strncmp(record, "NONE", 4)) {

         sprintf(record, " * Origin: %s (%d:%d/%d.%d)%c%c",
             echo_origin,
             from_node_zone,
             from_node_network,
             from_node_number,
             from_node_point,
             0x0d, 0x0a);

         plug_msg(record, strlen(record));
      }
      msg_out(0);
      mayhem_fclose(&msg_file, hold_message_file_name);
      if (to_node_number != (short)NIL) {
         c_out(WHITE, "Message sent...\n\r");
         did_mail = TRUE;
         return;
      }
      if ((mail_list = fopen(to_file_listing, "rt")) == (FILE *)NULL) {
         log_error(115);
         return;
      }
      c_out(WHITE, "Message being sent to all points...\n\r");
      copy_mail_to_all_nodes();
      return;
   }
   else if (record[0] == 'A') {
      c_out(WHITE, "\n\r\n\rMessage entry aborted.\n\r\n\r");
      mayhem_fclose(&msg_file, hold_message_file_name);
      unlink(hold_message_file_name);
      return;
   }
   else {
      goto ask_for_the_option;
   }
}

/* **********************************************************************
   * Take the data in the_buffer and plug it into the file.		*
   *									*
   ********************************************************************** */

void plug_msg(char *the_buffer, int char_count)
{
   char byte;

   for (count = 0; count < char_count; count++, the_buffer++) {
      byte = fputc(*the_buffer, msg_file);
      if (byte != *the_buffer) {
         c_out(LIGHTRED, "Unable to finish write of Subspace Message!\n\r");
         log_error(116);
         return;
      }
   }
}

/* **********************************************************************
   * A function is called rather than simply including it where it's	*
   * needed; this is so that and additional conditioning required in	*
   * the future may take place here.					*
   *									*
   ********************************************************************** */

void msg_out(char byte)
{
   char test;
        
   test = fputc(byte, msg_file);

   if (test != byte) {
      c_out(LIGHTRED, "Unable to finish write of Subspace Message!\n\r");
      log_error(117);
   }
}

/* **********************************************************************
   * Make a new message file.						*
   *									*
   ********************************************************************** */

unsigned short make_new_message(char statistics_message)
{
   int message_number;
   char arecord[201];
   char brecord[201];

   if (! statistics_message) {
      if (subspace_mail[strlen(subspace_mail) - 1] != '\\') {
         strcpy(arecord, subspace_mail);
         strcat(arecord, "\\");
      }
      else {
         strcpy(arecord, subspace_mail);
      }
   }
   else {
      strcpy(arecord, network_directory);
   }

   message_number = 1 + find_highest_message_number(arecord);

   original_message_number = message_number;
   sprintf(brecord, "%s%d.msg", arecord, message_number);
   strcpy(hold_message_file_name, brecord);

   if ((msg_file = mayhem_fopen(brecord, "wb", msg_file)) == (FILE *)NULL) {
      c_out(WHITE,
          "\n\rUnable to create your %s message. Problem unknown.\n\r",
          statistics_message ? "STATISTICS" : "subspace");
      return(FALSE);
   }

   return(TRUE);
}

/* **********************************************************************
   * See if a statistics package should be sent to me.                  *
   *                                                                    *
   * If not, simply return.                                             *
   *                                                                    *
   * If so, compose a statistics message and address it to the net and  *
   * node that's described in the configuration file.                   *
   *                                                                    *
   ********************************************************************** */

void mail_check_statistics_package(void)
{
   FILE *last_friday;
   char byte_test;

   if ((last_friday = fopen("ECHOSTAT.DAT", "rt")) == (FILE *)NULL) {
      if ((last_friday = fopen("ECHOSTAT.DAT", "wt")) == (FILE *)NULL) return;
      byte_test = fputs(the_date, last_friday);
      if (byte_test == (char)EOF) {
         log_error(120);
      }
      byte_test = fclose(last_friday);
      if (byte_test != 0) {
         log_error(124);
      }
      create_stat_package();
      return;
   }

   fgets(record, 200, last_friday);
   byte_test = fclose(last_friday);
   if (byte_test != 0) {
      log_error(125);
   }
   if (! strncmp(record, the_date, 1)) return;

   if ((last_friday = fopen("ECHOSTAT.DAT", "wt")) != (FILE *)NULL) {
      byte_test = fputs(the_date, last_friday);
      if (byte_test == (char)EOF) {
         log_error(121);
      }
      byte_test = fclose(last_friday);
      if (byte_test != 0) {
         log_error(126);
      }
      create_stat_package();
   }
}

/* **********************************************************************
   * Make copies of the message that was just create, plugging the      *
   * destination addresses as needed.                                   *
   *                                                                    *
   ********************************************************************** */

static void copy_mail_to_all_nodes(void)
{
   char arecord[201], brecord[201], hold_record[201];
   short a_message_number;
   char byte_test;

   if (subspace_mail[strlen(subspace_mail) - 1] != '\\') {
      strcpy(arecord, subspace_mail);
      strcat(arecord, "\\");
   }
   else {
      strcpy(arecord, subspace_mail);
   }

   while (! feof(mail_list)) {
      fgets(hold_record, 200, mail_list);
      if (! feof(mail_list)) {
         if (hold_record[0] != ';' && strlen(hold_record) > 3) {
            a_message_number = 1 + find_highest_message_number(arecord);

            sprintf(brecord, "copy %s%d.msg %s%d.msg > mayhem.tmp\r",
                arecord, original_message_number, arecord, a_message_number);

            system(brecord);

            sprintf(brecord, "%s%d.msg", arecord, a_message_number);

            if ((msg_file = mayhem_fopen(brecord, "r+b", msg_file)) == (FILE *)NULL) {
               c_out(WHITE, "\n\rUnable to create your subspace message. Problem unknown.\n\r");
               byte_test = fclose(mail_list);
               if (byte_test != 0) {
                  log_error(127);
               }
               return;
            }

            if (fread(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
               c_out(LIGHTRED, "\n\rUnable to write subspace mail!\n\r");
               byte_test = fclose(mail_list);
               if (byte_test != 0) {
                  log_error(128);
               }
               mayhem_fclose(&msg_file, hold_message_file_name);
               log_error(121);
               return;
            }

            strcpy(record, hold_record);
            process_destination_network_address();
            message.destination_node = to_node_number;
            message.destination_net = to_node_network;
            message.destination_zone = to_node_zone;
            message.destination_point = to_node_point;
            fseek(msg_file, 0, SEEK_SET);

            if (fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
               c_out(LIGHTRED, "\n\rUnable to write subspace mail!\n\r");
               byte_test = fclose(mail_list);
               if (byte_test != 0) {
                  log_error(129);
               }
               mayhem_fclose(&msg_file, hold_message_file_name);
               log_error(122);
               return;
            }

            mayhem_fclose(&msg_file, hold_message_file_name);
         }
      }
   }

/*
   Erase the original message addressed to NIL
*/

   sprintf(brecord, "del %s%d.msg\r", arecord, original_message_number);
   system(brecord);
   sprintf(brecord, "del mayhem.tmp\r");
   system(brecord);
   byte_test = fclose(mail_list);
   if (byte_test != 0) {
      log_error(130);
   }
   did_mail = TRUE;
}

/* **********************************************************************
   * Create a new message and compile some stats.                       *
   *                                                                    *
   * o The network address of your system                               *
   * o How many active ships your system currently has                  *
   * o Information on your systems strongest power player:              *
   *   o The name of the player                                         *
   *   o The strength of the player (in terms of power)                 *
   *   o Date and time the player was last active                       *
   * o Information on your systems highest planet ownership player:     *
   *   o The name of the player                                         *
   *   o The number of planets owned by that player                     *
   *   o Date and time the player was last active                       *
   * o Information on your systems highest killer player:               *
   *   o The name of the player                                         *
   *   o The number of kills that player has                            *
   *   o Date and time the player was last active                       *
   *                                                                    *
   * ------------------------------------------------------------------ *
   *                                                                    *
   * Take the ship that's the strongest ping and append its information *
   * to the statistics message after putting an end of file marker on   *
   * it.                                                                *
   *                                                                    *
   ********************************************************************** */

void create_stat_package(void)
{
    short s_count, l_count;
    unsigned long ship_power_value, test_value;
    short ship_kill_value;
    short ship_standing_value;
    char p_name[5], k_name[5], s_name[5];
    char *atpoint;
    char file_name[20];
    int how_many_command_files;
    FILE *attach_file, *list_file;
    char test_byte;
    unsigned long the_ping;
    char found_one;
    char duplication_name[81];
    FILE *dup_file;

    if (! make_new_message(TRUE)) return;

    strcpy(p_name, "NONE");
    strcpy(k_name, "NONE");
    strcpy(s_name, "NONE");

    c_out(WHITE,
        "Universal Mayhem Statistics Coordinator in process: %d.MSG\n\r",
        original_message_number);

    ship_power_value = ship_kill_value = 0;
    ship_standing_value = 0;

    strcpy(message.from, "Mayhem Statistics Coordinator");
    strcpy(message.to, "All Mayhem Sites");

    sprintf(message.subject, "%s%04X%04X.MHN",
        statistics_outbound_directory,
        from_node_network,
        from_node_number);

    message.date[0] = the_date[8];
    message.date[1] = the_date[9];
    message.date[2] = ' ';
    message.date[3] = the_date[4];
    message.date[4] = the_date[5];
    message.date[5] = the_date[6];
    message.date[6] = ' ';
    message.date[7] = the_date[22];
    message.date[8] = the_date[23];
    message.date[9] = ' ';
    message.date[10] = ' ';
    message.date[11] = (char)NULL;
    atpoint = the_date;
    atpoint += 11;
    strncat(message.date, atpoint, 8);

    message.cost = 0;

    message.originate_zone = from_node_zone;
    message.originate_net = from_node_network;
    message.originate_node = from_node_number;
    message.originate_point = from_node_point;

/*
    See if we have a LIST of addresses to send to
*/

    if (stat_node_zone == (short)NIL) {
        found_one = FALSE;
        message.destination_zone = from_node_zone;
        message.destination_net = from_node_network;
        message.destination_node = from_node_number;
        message.destination_point = from_node_point;

        if ((list_file = fopen(stat_file_name, "rt")) != (FILE *)NULL) {
            while (! feof(list_file) && !found_one) {
                fgets(record, 200, list_file);
                if (! feof(list_file)) {
                    point = record;
                    skipspace(point);
                    if (strlen(point) > 2 && *point != ';') {
                        message.destination_zone = find_delimiter(':');
                        message.destination_net = find_delimiter('/');
                        message.destination_node = find_delimiter('.');
                        message.destination_point = find_delimiter(' ');
                        found_one = TRUE;
                        strcpy(duplication_name, hold_message_file_name);
                    }
                }
            }
        }

        if (! found_one) {
            fclose(list_file);
        }
    }
    else {
        message.destination_zone = stat_node_zone;
        message.destination_net = stat_node_network;
        message.destination_node = stat_node_number;
        message.destination_point = stat_node_point;
    }

    message.reply = 0;

    if (! stat_hold) {   /* Crash Kill Local FileAttach */
        message.attribute = 2 + 16 + 256 + 128; 
    }
    else {               /* Crash Kill Hold Local FileAttach */
        message.attribute = 2 + 16 + 128 + 256 + 512;
    }

    message.upwards_reply = 0;
    message.a_character_1 = 1;
    strcpy(message.area_name, "AREA:MAYHEM");

    if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
        c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
        c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
        c_out(LIGHTRED, "No statistics message has been sent!\n\r");
        mayhem_fclose(&msg_file, hold_message_file_name);
        log_error(123);
        if (found_one) {
            fclose(list_file);
        }
        return;
    }

    msg_out(13);
    msg_out(10);

    msg_out(1);
    strcpy(record, ":stat:");
    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    sprintf(record, "Address %d:%d/%d.%d with %d active ships",
        from_node_zone,
        from_node_network,
        from_node_number,
        from_node_point,
        active_players);

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    how_many_command_files = 0;

    for (s_count = 1; s_count < players; s_count++) {
        read_enemy(s_count);
        if (enemy->ship_xpos != xsize / 2 && enemy->ship_xpos != ysize / 2) {
            test_value = enemy->ship_power + enemy->ship_shield + enemy->base_shield;
            if (enemy->local == 0) {
                if (command_file_exist(s_count)) {
                    how_many_command_files++;
                }
            }

            if (test_value > ship_power_value) {
                ship_power_value = test_value;
                strcpy(p_name, enemy->ship_name);
            }

            if (enemy->total_kills > ship_kill_value) {
                ship_kill_value = enemy->total_kills;
                strcpy(k_name, enemy->ship_name);
            }

            if (enemy->planets_owned > ship_standing_value) {
                ship_standing_value = enemy->planets_owned;
                strcpy(s_name, enemy->ship_name);
            }
        }
    }

    the_ping = ship_power_value / 1000000l;

    sprintf(record,
        "   [%s] with %ld 'ping' strength, [%s] with %d kills, [%s] with %d planets",
        p_name, the_ping,
        k_name, ship_kill_value,
        s_name, ship_standing_value);

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
    plug_msg(record, strlen(record));

    strcpy(record, network_origin);
    ucase(record);

    if (strncmp(record, "NONE", 4)) {

       sprintf(record, " * Origin: %s (%d:%d/%d.%d)%c%c",
          network_origin,
          from_node_zone,
          from_node_network,
          from_node_number,
          from_node_point,
          0x0d, 0x0a);

       plug_msg(record, strlen(record));
    }

    msg_out(0);                  /* End of message marker        */
    msg_out(26);                 /* End of file marker           */

    if (how_many_command_files == 0) {
        c_out(LIGHTGREEN, "\n\rThere are NO ships to export. None were found\n\r");
        c_out(LIGHTGREEN, "that has a *.SHP file to automate it!\n\r");
        message.attribute -= 16; /* Remove File Attach! */
        strcpy(message.subject, "-> No exports <-");
        rewind(msg_file);
        if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
           log_error(141);
        }
        mayhem_fclose(&msg_file, hold_message_file_name);
        did_mail = TRUE;
        if (found_one) {
            fclose(list_file);
        }
        return;
    }

    mayhem_fclose(&msg_file, hold_message_file_name);
    did_mail = TRUE;

    if ((attach_file = fopen(message.subject, "wb")) == (FILE *)NULL) {
        c_out(LIGHTRED, "Unable to create attach file: %s\n\r", message.subject);
        log_error(124);
        if (found_one) {
            fclose(list_file);
        }
        return;
    }

    test_byte = fputc(how_many_command_files, attach_file);
    if (test_byte != how_many_command_files) {
        c_out(LIGHTRED, "Unable to continue to write attach file data!\n\r");
        log_error(118);
        if (found_one) {
            fclose(list_file);
        }
        return;
    }

    for (s_count = 1; s_count < players; s_count++) {
        read_enemy(s_count);
        if (enemy->ship_xpos != xsize / 2 && enemy->ship_xpos != ysize / 2) {
            strcpy(p_name, enemy->ship_name);
            if (command_file_exist(s_count)) {
                if (enemy->local == 0) {
                    strcpy(enemy->ship_pass, "{{{{");
                    enemy->local = 1;
                    if (mayhem_fwrite(enemy, sizeof(struct ship_file), 1, attach_file) != 1) {
                        c_out(LIGHTRED, "\n\rTROUBLE: write to attach file '%s' failed!\n\r", message.subject);
                        c_out(LIGHTRED, "\n\rTROUBLE: Attach file pointer: %p\n\r", attach_file);
                        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
                        c_out(LIGHTRED, "Export did not continue!\n\r");
                        mayhem_fclose(&attach_file, message.subject);
                        log_error(125);
                        if (found_one) {
                            fclose(list_file);
                        }
                        return;
                   }
                    STRNCPY(file_name, hold[s_count]->names, 4);
                    file_name[4] = (char)NULL;
                    strcat(file_name, ".SHP");
                    if ((command_file = mayhem_fopen(file_name, "rt", command_file)) != (FILE *)NULL) {
                        while (! feof(command_file)) {
                            fgets(record, 200, command_file);
                            if (! feof(command_file)) {
                                test_byte = fputs(record, attach_file);
                                if (test_byte == (char)EOF) {
                                   log_error(122);
                                }
                            }
                        }
                        mayhem_fclose(&command_file, file_name);
                    }
                    else {
                        c_out(LIGHTRED, "\n\rTROUBLE: couldn't find command file '%s'\n\r", file_name);
                        log_error(126);
                        if (found_one) {
                            fclose(list_file);
                        }
                        return;
                    }           /* End of user information */
                    test_byte = fputc(123, attach_file);
                    if (test_byte != 123) {
                        c_out(LIGHTRED, "Unable to continue to write attach file data!\n\r");
                        log_error(119);
                        if (found_one) {
                            fclose(list_file);
                        }
                        return;
                    }
                }
            }
        }
    }

    test_byte = fclose(attach_file);
    if (test_byte != 0) {
        log_error(131);
    }

    c_out(LIGHTGREEN, "---> There were %d exports in file: %s\n\r",
        how_many_command_files, message.subject);

/*
    If there was a list of network addresses to send to, extract the
    remaining information and make copies!
*/

    if (! found_one) {
        return;
    }

    if ((dup_file = fopen(duplication_name, "rb")) == (FILE *)NULL) {
        return;
    }

    while (! feof(list_file)) {
        fgets(record, 200, list_file);
        if (! feof(list_file)) {
            point = record;
            skipspace(point);
            if (strlen(point) > 2 && *point != ';') {
                stat_node_zone = find_delimiter(':');
                stat_node_network = find_delimiter('/');
                stat_node_number = find_delimiter('.');
                stat_node_point = find_delimiter(' ');

                if (! make_new_message(TRUE)) {
                    fclose(list_file);
                    fclose(dup_file);
                    return;
                }

                if (fread(&message, sizeof(struct fido_msg), 1, dup_file) != 1) {
                    fclose(list_file);
                    fclose(dup_file);
                    return;
                }

                message.destination_zone = stat_node_zone;
                message.destination_net = stat_node_network;
                message.destination_node = stat_node_number;
                message.destination_point = stat_node_point;

                if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
                    fclose(list_file);
                    fclose(dup_file);
                    return;
                }

                while (! feof(dup_file)) {
                    fgets(record, 200, dup_file);
                    if (! feof(dup_file)) {
                        fputs(record, msg_file);
                    }
                }

                c_out(LIGHTGREEN, "Copies to %d:%d/%d.%d\n\r",
                    message.destination_zone,
                    message.destination_net,
                    message.destination_node,
                    message.destination_point);

                mayhem_fclose(&msg_file, hold_message_file_name);
                rewind(dup_file);
            }
        }
    }

    fclose(dup_file);
    fclose(list_file);
}

/* **********************************************************************
   * Here we will invoke the stand-alone utility that will scan the     *
   * STATS.SHP file for the remote players.                             *
   *                                                                    *
   ********************************************************************** */

void import_statistics_file(void)
{
    if (statistics_enabled) {
       perform_quit(999);

       if (crash_reset) {
           setvect(0x1c, old_interrupt);
       }

       execl("UMREMOTE.EXE",
          "UMREMOTE",
          "Mayhem",
          want_color ? "Y" : "N",
          color_enable ? "Y" : "N",
          NULL);
    }
}

/* **********************************************************************
   * A ship or base has been killed.                                    *
   *                                                                    *
   * Create a message package for it.                                   *
   *                                                                    *
   * Killer -           Ship name that did the killing                  *
   *                                                                    *
   * Killed -           Ship or base name that got killed               *
   *                                                                    *
   * Killed_A_Ship -    TRUE if it was a ship, FALSE if it was a base   *
   *                                                                    *
   * How -              0 - Slaver Death triggered                      *
   *                    1 - Torpedo fire                                *
   *                    2 - Phaser fire                                 *
   *                    3 - Command file directive                      *
   *                                                                    *
   ********************************************************************** */

void inform_kill(char *killer, char *killed, char killed_a_ship, char how)
{
    char *atpoint;
        
    if (! make_new_message(FALSE)) return;

    strcpy(message.from, "Mayhem Killer Information");
    strcpy(message.to, "All Mayhem Sites");
    sprintf(message.subject, "Death of a player!");

    message.date[0] = the_date[8];
    message.date[1] = the_date[9];
    message.date[2] = ' ';
    message.date[3] = the_date[4];
    message.date[4] = the_date[5];
    message.date[5] = the_date[6];
    message.date[6] = ' ';
    message.date[7] = the_date[22];
    message.date[8] = the_date[23];
    message.date[9] = ' ';
    message.date[10] = ' ';
    message.date[11] = (char)NULL;
    atpoint = the_date;
    atpoint += 11;
    strncat(message.date, atpoint, 8);

    message.destination_node = stat_node_number;
    message.originate_node = from_node_number;
    message.cost = 0;
    message.originate_net = from_node_network;
    message.destination_net = stat_node_network;
    message.destination_zone = stat_node_zone;
    message.originate_zone = from_node_zone;
    message.destination_point = stat_node_point;
    message.originate_point = from_node_point;
    message.reply = 0;
    message.attribute = 2 + 256; /* Crash Local */
    message.upwards_reply = 0;
    message.a_character_1 = 1;
    strcpy(message.area_name, "AREA:MAYHEM");

    if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
        c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
        c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
        c_out(LIGHTRED, "Can't inform player about the kill!\n\r");
        mayhem_fclose(&msg_file, hold_message_file_name);
        log_error(127);
        return;
    }

    msg_out(13);
    msg_out(10);

    sprintf(record,
        "Ship [%s] destroyed %s [%s] this day on system %d:%d/%d.%d",
        killer,
        killed,
        killed_a_ship ? "ship" : "base",
        from_node_zone,
        from_node_network,
        from_node_number,
        from_node_point);

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    strcpy(record, "Destroyed by ");

    switch(how) {
        case 0: strcat(record, "slaver death weapon!"); break;
        case 1: strcat(record, "torpedo fire!"); break;
        case 2: strcat(record, "phaser fire!"); break;
        case 3: strcat(record, "an automated ship!"); break;
    }

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
    plug_msg(record, strlen(record));

    strcpy(record, echo_origin);
    ucase(record);

    if (strncmp(record, "NONE", 4)) {

       sprintf(record, " * Origin: (%d:%d/%d.%d)%c%c",
          echo_origin,
          from_node_zone,
          from_node_network,
          from_node_number,
          from_node_point,
          0x0d, 0x0a);

       plug_msg(record, strlen(record));
    }

    msg_out(0);                  /* End of message marker        */
    msg_out(26);                 /* End of file marker           */

    mayhem_fclose(&msg_file, hold_message_file_name);
    did_mail = TRUE;
}

/* **********************************************************************
   * Find the highest message number.                                   *
   *                                                                    *
   ********************************************************************** */

static short find_highest_message_number(char *directory)
{
    char result;
    short highest_message_number = 0;
    char directory_search[100];

    strcpy(directory_search, directory);
    strcat(directory_search, "*.msg");

    result = findfirst(directory_search, &file_block, 0x16);

    if (! result) {
        if (atoi(file_block.ff_name) > highest_message_number) {
            highest_message_number = atoi(file_block.ff_name);
        }
    }

    while (! result) {
        result = findnext(&file_block);
        if (! result) {
            if (atoi(file_block.ff_name) > highest_message_number) {
                highest_message_number = atoi(file_block.ff_name);
            }
        }
    }

    return(highest_message_number);
}

/* **********************************************************************
   * The current ship shoved a player around!                           *
   *                                                                    *
   ********************************************************************** */

void mail_leash(short enemy_number,
    long fromx,
    long fromy,
    long tox,
    long toy,
    char how)
{
    char *atpoint;

    if (enemy_number == user_number) return;
        
    if (! make_new_message(FALSE)) return;

    strcpy(message.from, "Mayhem Leash Information");
    strcpy(message.to, "All Mayhem Sites");

    sprintf(message.subject, "A Player has been %s around!",
        how ? "Dragged" : "Shoved");

    message.date[0] = the_date[8];
    message.date[1] = the_date[9];
    message.date[2] = ' ';
    message.date[3] = the_date[4];
    message.date[4] = the_date[5];
    message.date[5] = the_date[6];
    message.date[6] = ' ';
    message.date[7] = the_date[22];
    message.date[8] = the_date[23];
    message.date[9] = ' ';
    message.date[10] = ' ';
    message.date[11] = (char)NULL;
    atpoint = the_date;
    atpoint += 11;
    strncat(message.date, atpoint, 8);

    message.destination_node = stat_node_number;
    message.originate_node = from_node_number;
    message.cost = 0;
    message.originate_net = from_node_network;
    message.destination_net = stat_node_network;
    message.destination_zone = stat_node_zone;
    message.originate_zone = from_node_zone;
    message.destination_point = stat_node_point;
    message.originate_point = from_node_point;
    message.reply = 0;
    message.attribute = 2 + 256; /* Crash Local */
    message.upwards_reply = 0;
    message.a_character_1 = 1;
    strcpy(message.area_name, "AREA:MAYHEM");

    if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
        c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
        c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
        c_out(LIGHTRED, "Can't inform player about the kill!\n\r");
        mayhem_fclose(&msg_file, hold_message_file_name);
        log_error(127);
        return;
    }

    msg_out(13);
    msg_out(10);

    sprintf(record,
        "Ship [%s] Leashed %s on system %d:%d/%d.%d from {%ld-%ld} to {%ld-%ld}",
        hold[user_number]->names,
        hold[enemy_number]->names,
        from_node_zone,
        from_node_network,
        from_node_number,
        from_node_point,
        fromx, fromy,
        tox, toy);

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);
    msg_out(13);
    msg_out(10);

    sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
    plug_msg(record, strlen(record));

    strcpy(record, echo_origin);
    ucase(record);

    if (strncmp(record, "NONE", 4)) {

       sprintf(record, " * Origin: %s (%d:%d/%d.%d)%c%c",
          echo_origin,
          from_node_zone,
          from_node_network,
          from_node_number,
          from_node_point,
          0x0d, 0x0a);

       plug_msg(record, strlen(record));
    }

    msg_out(0);                  /* End of message marker        */
    msg_out(26);                 /* End of file marker           */

    mayhem_fclose(&msg_file, hold_message_file_name);
    did_mail = TRUE;

}

/* **********************************************************************
   * The current ship boarded a ship or a base.                         *
   *                                                                    *
   ********************************************************************** */

void mail_board(short enemy_number,
   char boarded_ship,
   UL took_power,
   UL took_credits,
   UL took_cargo,
   UL took_torps,
   char friendly)
{
    char *atpoint;

    if (enemy_number == user_number) return;
        
    if (! make_new_message(FALSE)) return;

    strcpy(message.from, "Mayhem Board Information");
    strcpy(message.to, "All Mayhem Sites");

    if (! friendly) {
        sprintf(message.subject, "A Players %s has been boarded!",
            boarded_ship ? "SHIP" : "BASE");
    }
    else {
        sprintf(message.subject, "Player resupplied at friendly base!");
    }

    message.date[0] = the_date[8];
    message.date[1] = the_date[9];
    message.date[2] = ' ';
    message.date[3] = the_date[4];
    message.date[4] = the_date[5];
    message.date[5] = the_date[6];
    message.date[6] = ' ';
    message.date[7] = the_date[22];
    message.date[8] = the_date[23];
    message.date[9] = ' ';
    message.date[10] = ' ';
    message.date[11] = (char)NULL;
    atpoint = the_date;
    atpoint += 11;
    strncat(message.date, atpoint, 8);

    message.destination_node = stat_node_number;
    message.originate_node = from_node_number;
    message.cost = 0;
    message.originate_net = from_node_network;
    message.destination_net = stat_node_network;
    message.destination_zone = stat_node_zone;
    message.originate_zone = from_node_zone;
    message.destination_point = stat_node_point;
    message.originate_point = from_node_point;
    message.reply = 0;
    message.attribute = 2 + 256; /* Crash Local */
    message.upwards_reply = 0;
    message.a_character_1 = 1;
    strcpy(message.area_name, "AREA:MAYHEM");

    if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
        c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
        c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
        c_out(LIGHTRED, "Can't inform player about the kill!\n\r");
        mayhem_fclose(&msg_file, hold_message_file_name);
        log_error(127);
        return;
    }

    msg_out(13);
    msg_out(10);

    if (! friendly) {
        sprintf(record,
            "Ship [%s] Boarded [%s] on system %d:%d/%d.%d!",
            hold[user_number]->names,
            hold[enemy_number]->names,
            from_node_zone,
            from_node_network,
            from_node_number,
            from_node_point);
    }
    else {
        sprintf(record,
            "Ship [%s] Resupplied from base [%s] on system %d:%d/%d.%d!",
            hold[user_number]->names,
            hold[enemy_number]->names,
            from_node_zone,
            from_node_network,
            from_node_number,
            from_node_point);
    }

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);

    if (! friendly) {
        if (boarded_ship) {
            sprintf(record, "Spoils: Power %ld", took_power);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
            sprintf(record, "        Credits: %ld", took_credits);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
            sprintf(record, "        Cargo: %ld", took_cargo);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
            sprintf(record, "        Torpedoes: %ld", took_torps);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
        }
        else {
            sprintf(record, "Spoils: Credits %ld", took_credits);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
            sprintf(record, "        Cargo: %ld", took_cargo);
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
            msg_out(13);
            msg_out(10);
            sprintf(record, "All enemy base crew members were killed in the action!");
            plug_msg(record, strlen(record));
            msg_out(13);
            msg_out(10);
        }
    }

    msg_out(13);
    msg_out(10);

    sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
    plug_msg(record, strlen(record));

    strcpy(record, echo_origin);
    ucase(record);

    if (strncmp(record, "NONE", 4)) {

       sprintf(record, " * Origin: %s (%d:%d/%d.%d)%c%c",
          echo_origin,
          from_node_zone,
          from_node_network,
          from_node_number,
          from_node_point,
          0x0d, 0x0a);

       plug_msg(record, strlen(record));
    }

    msg_out(0);                  /* End of message marker        */
    msg_out(26);                 /* End of file marker           */

    mayhem_fclose(&msg_file, hold_message_file_name);
    did_mail = TRUE;
}

/* **********************************************************************
   * The current player picked up a slaver piece! Tell everyone!        *
   *                                                                    *
   ********************************************************************** */

void mail_slaver(char piece)
{
    char *atpoint;

    if (! make_new_message(FALSE)) return;

    strcpy(message.from, "Mayhem Slaver Information");
    strcpy(message.to, "All Mayhem Sites");

    sprintf(message.subject, "A Player has found a Slaver part!");

    message.date[0] = the_date[8];
    message.date[1] = the_date[9];
    message.date[2] = ' ';
    message.date[3] = the_date[4];
    message.date[4] = the_date[5];
    message.date[5] = the_date[6];
    message.date[6] = ' ';
    message.date[7] = the_date[22];
    message.date[8] = the_date[23];
    message.date[9] = ' ';
    message.date[10] = ' ';
    message.date[11] = (char)NULL;
    atpoint = the_date;
    atpoint += 11;
    strncat(message.date, atpoint, 8);

    message.destination_node = stat_node_number;
    message.originate_node = from_node_number;
    message.cost = 0;
    message.originate_net = from_node_network;
    message.destination_net = stat_node_network;
    message.destination_zone = stat_node_zone;
    message.originate_zone = from_node_zone;
    message.destination_point = stat_node_point;
    message.originate_point = from_node_point;
    message.reply = 0;
    message.attribute = 2 + 256; /* Crash Local */
    message.upwards_reply = 0;
    message.a_character_1 = 1;
    strcpy(message.area_name, "AREA:MAYHEM");

    if (mayhem_fwrite(&message, sizeof(struct fido_msg), 1, msg_file) != 1) {
        c_out(LIGHTRED, "\n\rTROUBLE: Can't write message header information!\n\r");
        c_out(LIGHTRED, "\n\rTROUBLE: Message file pointer: %p\n\r", msg_file);
        c_out(LIGHTRED, "\n\rTROUBLE: Files opened: %d\n\r", files_opened);
        c_out(LIGHTRED, "Can't inform player about the kill!\n\r");
        mayhem_fclose(&msg_file, hold_message_file_name);
        log_error(127);
        return;
    }

    msg_out(13);
    msg_out(10);

    sprintf(record,
        "Ship [%s] on system %d:%d/%d.%d found part %d: %s!",
        hold[user_number]->names,
        from_node_zone,
        from_node_network,
        from_node_number,
        from_node_point,
        piece,
        goal_item_description[piece]);

    plug_msg(record, strlen(record));
    msg_out(13);
    msg_out(10);
    msg_out(13);
    msg_out(10);

    sprintf(record, "--- Mayhem %s%c\n", THE_VERSION, 0x0d);
    plug_msg(record, strlen(record));

    strcpy(record, echo_origin);
    ucase(record);

    if (strncmp(record, "NONE", 4)) {

       sprintf(record, " * Origin: %s (%d:%d/%d.%d)%c%c",
          echo_origin,
          from_node_zone,
          from_node_network,
          from_node_number,
          from_node_point,
          0x0d, 0x0a);

       plug_msg(record, strlen(record));
    }

    msg_out(0);                  /* End of message marker        */
    msg_out(26);                 /* End of file marker           */

    mayhem_fclose(&msg_file, hold_message_file_name);
    did_mail = TRUE;
}


