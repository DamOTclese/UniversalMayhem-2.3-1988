
/* **********************************************************************
   * free.c								*
   *									*
   * Copyrite 1988, 1989, 1990, 1991.                                   *
   * Fredric L. Rice. All rights reserved.                              *
   *									*
   ********************************************************************** */

#include "defines.h"
#include "alloc.h"
#include "stdio.h"
#include "function.h"
#include "ship.h"
#include "io.h"
#include "stdarg.h"
#include "conio.h"
#include "goal.h"

/* **********************************************************************
   * Define the external and local data that this module needs to       *
   * access.                                                            *
   *                                                                    *
   ********************************************************************** */

   extern char to_save;
   extern char allow_mail[4];
   extern char *subspace_mail;
   extern UL total_allocated, smallest_allocated, largest_allocated;
   extern char is_redirected;
   extern char *record;
   extern char files_opened, highest_opened, *file_names[20];
   extern char crash_reset;
   extern char interrupted_serial;
   extern char ham_version;
   extern unsigned long buffered;
   extern char interrupt_enable;
   extern unsigned long recursed, over_run;
   extern unsigned long transmit_overflow;
   extern unsigned long missed_tx;

   extern void offer_data(void);

/* **********************************************************************
   * An undocumended command is the FREE command. It tells the operator *
   * and the programmer how much free memory there is.			*
   *									*
   ********************************************************************** */

void perform_free(void)
{
   unsigned long amount_left;
   char *atpoint;
   char g_count, count;

   amount_left = (unsigned long) farcoreleft();

   c_out(WHITE, "\n\rFree number of bytes left in heap: %ld\n\r",
      (unsigned long) amount_left);

   c_out(WHITE, "Subspace mail goes to %s\n\r", subspace_mail);

   if (to_save == 0) {
      c_out(WHITE, "Running with planets nameable\n\r");
   }
   else {
      c_out(WHITE, "Planets are not nameable\n\r");
   }

   c_out(WHITE, "Subspace mail is %s\n\r", allow_mail);

   c_out(WHITE, "There is %ld bytes of allocated memory in use.\n\r",
      memory_used());

   c_out(WHITE, "There are %d files opened, %d was the highest\n\r",
      files_opened, highest_opened);

   for (count = 0; count < 20; count++)
      if (file_names[count] != (char *)NULL)
         c_out(WHITE, "   File: %s still opened\n\r", file_names[count]);

   if (crash_reset)
       c_out(LIGHTRED,
           "The keyboard will reset the computer if Mayhem crashes\n\r");

   if (ham_version) {
      c_out(LIGHTBLUE,
          "Running HAM Radio Version! Keyboard will not time out!\n\r");
   }
   else {
      c_out(LIGHTBLUE, "Not running Mayhem over a HAM Radio.\n\r");
   }

   if (is_redirected == 1) {
       c_out(LIGHTGREEN, "Interrupted serial I/O status: ");
       if (interrupted_serial) {
          c_out(WHITE, "ENABLED\n\r");

          c_out(WHITE,
              "%ld buffered %ld recursions %ld runs %ld suspensions, missed %ld\n\r",
              buffered, recursed, over_run, transmit_overflow, missed_tx);
       }
       else {
          if (interrupt_enable) {
             c_out(WHITE, "DISABLED\n\r");
          }
          else {
             c_out(WHITE, "TURNED OFF\n\r");
          }
       }
   }

   c_out(WHITE, "Size of players data record: %d bytes\n\r",
       sizeof(struct ship_file));

   atpoint = record;
   atpoint += 4;
   skipspace(atpoint);

   if (*atpoint == '^') {
      for (g_count = 0; g_count < 10; g_count++) {
         if (goal_item[g_count] != (struct goal_elements *)NULL) {
            c_out(LIGHTGREEN, "Item %02d) {%d-%d} Universe [%d]",
               g_count,
               goal_item[g_count]->goal_xpos,
               goal_item[g_count]->goal_ypos,
               goal_item[g_count]->goal_universe);
            if (goal_item[g_count]->goal_on_ship == (char)NIL) {
               c_out(LIGHTGREEN, "\n\r");
            }
            else {
               c_out(LIGHTRED, "On ship %d\n\r", goal_item[g_count]->goal_on_ship);
            }
         }
      }
   }
   else if (toupper(*atpoint) == 'C') {
      recursed = over_run = transmit_overflow = missed_tx = 0l;
   }
}

/* **********************************************************************
   * Add the amount of memory allocated to the total and check to see   *
   * if the block is the smallest one yet, or the largest one yet.      *
   *                                                                    *
   ********************************************************************** */

void memory_allocated(UL this_amount)
{
   total_allocated += this_amount;
   if (this_amount < smallest_allocated) smallest_allocated = this_amount;
   if (this_amount > largest_allocated) largest_allocated = this_amount;
}

/* **********************************************************************
   * Subtract the amount of memory from the total.                      *
   *                                                                    *
   ********************************************************************** */

void memory_freed(UL this_amount)
{
   total_allocated -= this_amount;
}

/* **********************************************************************
   * Return the amount of memory still allocated.                       *
   *                                                                    *
   ********************************************************************** */

UL memory_used(void)
{
   return(total_allocated);
}


