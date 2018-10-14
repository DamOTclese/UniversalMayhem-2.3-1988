
/* **********************************************************************
   * leash.c                                                            *
   *                                                                    *
   * Copyrite 1990, 1991,                                               *
   * Fredric L. Rice. All rights reserved.                              *
   *                                                                    *
   * Allow the leash commands to work.                                  *
   *                                                                    *
   ********************************************************************** */

#include "defines.h"
#include "stdio.h"
#include "functions.h"
#include "ship.h"
#include "holder.h"
#include "conio.h"
#include "string.h"
#include "stdlib.h"

/* **********************************************************************
   * Define the external and local data that this module needs to       *
   * access.                                                            *
   *                                                                    *
   ********************************************************************** */

   extern char *record;
   extern short directions[18];
   extern char drag_ship;
   extern short players;
   extern short ship_count;
   extern unsigned short close_ship[TOTAL_PLAYERS];
   extern short user_number;
   extern long xsize, ysize;
   extern unsigned short the_percent;
   extern shoved_flag;

/* **********************************************************************
   * Define the functions that are used only here.                      *
   *                                                                    *
   ********************************************************************** */

   static void leash_tight(short enemy_number);
   static void leash_shove(short enemy_number);
   static void leash_drag(short enemy_number);
   static void leash_default(void);

/* **********************************************************************
   * See if the name of the ship offered is valid to use.               *
   *                                                                    *
   * Return value:                                                      *
   *            0 - Ship is good. Use it.                               *
   *            1 - Ship not found, don't use it.                       *
   *            2 - Ship was destroyed! Don't use it.                   *
   *            3 - Ship is already leashed. Don't use it.              *
   *            4 - The active ship already has a leash! Don't use it.  *
   *            5 - Enemy ship isn't in the area. Don't use it.         *
   *            6 - Tried to leash onto a cop! Don't use it.            *
   *            7 - Defend or Assisting. Don't use it.                  *
   *                                                                    *
   ********************************************************************** */

static char valid_ship_name(char *this_ship, short *enemy_number)
{
    short ship_loop, scanning;

    for (ship_loop = 0; ship_loop < players; ship_loop++) {
        if (! strncmp(this_ship, hold[ship_loop]->names, 4)) {
            if (ship_loop == 0) {
                return(6);
            }
            if (hold[ship_loop]->sxpos == 0 && hold[ship_loop]->sypos == 0) {
                return(2);
            }
            read_enemy(ship_loop);
            if (enemy->leashed_by != (char)NIL) {
                return(3);
            }
            if (ships->leashed_to != (char)NIL) {
                return(4);
            }
            if (assisting_ship(ship_loop)) {
                return(7);
            }
            plug_close_objects(FALSE);
            for (scanning = 0; scanning < ship_count; scanning++) {
                if (close_ship[scanning] == ship_loop) {
                    *enemy_number = ship_loop;
                    return(0);
                }
            }
            return(5);
        }
    }

    return(1);
}

/* **********************************************************************
   * Perform the leash functions.                                       *
   *                                                                    *
   * t - Requesting a tight leash                                       *
   * s - Requestiong a shove                                            *
   * d - Requesting a drag                                              *
   * {Default information}                                              *
   *                                                                    *
   * If a ship name follows, use it, otherwise ask for it.              *
   *                                                                    *
   * o tight      - Your ship is tractored to the enemy ship for the    *
   *                rest of the day. If your enemy ships captain gets   *
   *                into the system, your ship will continue to be      *
   *                leashed to his ship for his play period. The leash  *
   *                is broken when your ship or the enemy ship bails    *
   *                out of a fight according to its command file        *
   *                directives. This means that you and the enemy ship  *
   *                may fight each other.                               *
   *                                                                    *
   * o shove      - Your ship may shove the enemy ship in any direction *
   *                you choose. Its warp engines won't be able to stop  *
   *                your tractors unless emergency power is applied as  *
   *                a bail-out according to the enemy ships or your     *
   *                ships command file directives. You and the enemy    *
   *                may fire at each other in this configuration.       *
   *                                                                    *
   * o drag       - Your ship may drag the enemy ship for a certain     *
   *                amount of distance until the enemy ships engines    *
   *                warp up to enough power to pull away from you. You  *
   *                may not fire at the enemy ship in this configuration*
   *                but it may fire on you. You may not pull an enemy   *
   *                ship through a gateway.                             *
   *                                                                    *
   ********************************************************************** */

void perform_leash(void)
{
    char *atpoint;
    char leash_type;
    char to_leash[5], found_ship, valid;
    short enemy_number;

    ucase(record);
    atpoint = record;
    atpoint += 5;
    skipspace(atpoint);
    leash_type = *atpoint;

    if (leash_type == (char)NULL) {
        leash_default();
        return;
    }

    if (leash_type == 'T' || leash_type == 'S' || leash_type == 'D') {
        atpoint++;
        skipspace(atpoint);

        if (*atpoint != (char)NULL) {
            if (strlen(atpoint) > 3) {
                STRNCPY(to_leash, atpoint, 4);
                to_leash[4] = (char)NULL;
                found_ship = TRUE;
            }
            else {
                found_ship = FALSE;
            }
        }
        else {
            found_ship = FALSE;
        }

        if (! found_ship) {
            c_out(WHITE, "Enter ship to leash on to: ");
            timed_input(FALSE);
            ucase(record);
            STRNCPY(to_leash, record, 4);
        }

        if (! strcmp(to_leash, hold[user_number]->names)) {
            c_out(LIGHTRED, "You can't leash onto yourself!\n\r");
            return;
        }

        valid = valid_ship_name(to_leash, &enemy_number);

        if (valid == 1) {
            c_out(LIGHTRED, "Ship '%s' doesn't exist!\n\r", to_leash);
            return;
        }
        else if (valid == 2) {
            c_out(LIGHTRED, "Ship '%s' has been destroyed!\n\r", to_leash);
            return;
        }
        else if (valid == 3) {
            c_out(LIGHTRED, "Ship '%s' is already leashed by %s\n\r",
                to_leash, hold[enemy->leashed_by]->names);
            return;
        }
        else if (valid == 4) {
            c_out(LIGHTRED, "Your ship is already leashed to %s\n\r",
                hold[enemy->leashed_to]->names);
            return;
        }
        else if (valid == 5) {
            c_out(LIGHTRED, "Ship '%s' isn't within tractor range!\n\r", to_leash);
            return;
        }
        else if (valid == 6) {
            c_out(LIGHTRED, "The cops don't like to be shoved around!\n\r");
            c_out(LIGHTBLUE, "Leashed has not been performed.\n\r");
            return;
        }
        else if (valid == 7) {
            c_out(WHITE, "That ship is set to ASSIST or DEFEND you!\n\r");
            c_out(WHITE, "Your computer wll not allow tractors to engage!\n\r");
            return;
        }

        switch(leash_type) {
            case 'T': leash_tight(enemy_number); break;
            case 'S': leash_shove(enemy_number); break;
            case 'D': leash_drag(enemy_number); break;
        }
        return;
    }

    c_out(LIGHTRED, "Leash{t||s||d} {ship}   or   Leash {enter}\n\r");
    c_out(LIGHTRED, "   t - tight, s - shove, d - drag\n\r");
}

/* **********************************************************************
   * Perform a tight leash.                                             *
   *                                                                    *
   * o tight      - Your ship is tractored to the enemy ship for the    *
   *                rest of the day. If your enemy ships captain gets   *
   *                into the system, your ship will continue to be      *
   *                leashed to his ship for his play period. The leash  *
   *                is broken when your ship or the enemy ship bails    *
   *                out of a fight according to its command file        *
   *                directives. This means that you and the enemy ship  *
   *                may fight each other.                               *
   *                                                                    *
   ********************************************************************** */

static void leash_tight(short enemy_number)
{
    enemy->leashed_by = (char)user_number;
    ships->leashed_to = (char)enemy_number;
    write_enemy(enemy_number);
    write_user();

    c_out(WHITE, "You are tightly leashed to ship %s\n\r",
        hold[enemy_number]->names);
}

/* **********************************************************************
   * Perform a shove leash.                                             *
   *                                                                    *
   * o shove      - Your ship may shove the enemy ship in any direction *
   *                you choose. Its warp engines won't be able to stop  *
   *                your tractors unless emergency power is applied as  *
   *                a bail-out according to the enemy ships or your     *
   *                ships command file directives. You and the enemy    *
   *                may fire at each other in this configuration.       *
   *                                                                    *
   ********************************************************************** */

static void leash_shove(short enemy_number)
{
    char direction;
    char shove;
    long new_x, new_y;
    char hold_index;

    c_out(LIGHTBLUE, "Enter the direction to shove ship %s: ",
        hold[enemy_number]->names);

    timed_input(FALSE);
    direction = atoi(record);

    if (direction < 1 || direction > 9 || direction == 5) {
        c_out(LIGHTRED, "Invalid direction! (1 through 9. Not 5)\n\r");
        c_out(WHITE, "Shove has not been performed!\n\r");
        return;
    }

    c_out(LIGHTBLUE, "Enter shove factor: (From 1 to 100, 100 maximum): ");
    timed_input(FALSE);
    shove = atoi(record);

    if (shove < 1 || shove > 100) {
        c_out(LIGHTRED,
            "The shove factor is 1 to 100 with 100 being the greatest!\n\r");
        return;
    }

    hold_index = ((direction - 1) * 2);

    new_x = enemy->ship_xpos + (long)shove * directions[hold_index];
    new_y = enemy->ship_ypos + (long)shove * directions[++hold_index];

    if (new_x < 1 || new_x > xsize - 1 || new_y < 1 || new_y > ysize - 1) {
        c_out(LIGHTBLUE, "That would shove it right out of the universe!\n\r");
        return;
    }

    if ((shove * 500000L) > ships->ship_power) {
        c_out(LIGHTRED,
            "You need %ld units of power for that shove value!\n\r",
            500000L * shove);
            return;
    }

    read_enemy(enemy_number);
    return_torp(enemy_number);
    return_torp(enemy_number);
    return_torp(enemy_number);
    return_torp(enemy_number);

    c_out(WHITE, "Ship %s has been shoved from {%ld-%ld} to {%ld-%ld}\n\r",
        hold[enemy_number]->names,
        enemy->ship_xpos, enemy->ship_ypos,
        new_x, new_y);

    if (! shoved_flag) {
        mail_leash(enemy_number, enemy->ship_xpos, enemy->ship_ypos, new_x, new_y, 1);
        shoved_flag = TRUE;
    }
    enemy->ship_xpos = new_x;
    enemy->ship_ypos = new_y;
    hold[enemy_number]->sxpos = new_x;
    hold[enemy_number]->sypos = new_y;
    write_enemy(enemy_number);
    ships->ship_power -= (500000 * shove);
    write_user();
    ship_attacking(enemy_number);
}

/* **********************************************************************
   * Perform a drag leash.                                              *
   *                                                                    *
   * o drag       - Your ship may drag the enemy ship for a certain     *
   *                amount of distance until the enemy ships engines    *
   *                warp up to enough power to pull away from you. You  *
   *                may not fire at the enemy ship in this configuration*
   *                but it may fire on you. You may not pull an enemy   *
   *                ship through a gateway.                             *
   *                                                                    *
   ********************************************************************** */

static void leash_drag(short enemy_number)
{
    if (drag_ship != (char)NIL) {
        c_out(LIGHTRED, "You are already set to drag an enemy ship!\n\r");
        return;
    }

    drag_ship = enemy_number;
    c_out(WHITE, "All set to drag enemy ship with you on your next warp!\n\r");
}

/* **********************************************************************
   * See if the player wants to break the leash.                        *
   *                                                                    *
   ********************************************************************** */

static void see_if_break_leash(void)
{
    c_out(LIGHTBLUE, "Do you want to break the leash you have on %s? ",
        hold[ships->leashed_to]->names);

    timed_input(FALSE);
    ucase(record);
    if (record[0] != 'Y') return;

    c_out(LIGHTRED,
        "This will take a lot of power and could cause some damage!\n\r");

    c_out(LIGHTRED, "Are you sure you want to try to break it? ");
    timed_input(FALSE);
    ucase(record);
    if (record[0] != 'Y') return;
    bounce_off_object();
    the_percent = 90;
    c_out(LIGHTRED, "\n\r!!! Breakaway! Breakaway! !!!\n\r");
    make_some_damage(user_number, (short)NIL);
}

/* **********************************************************************
   * Perform a default leash.                                           *
   *                                                                    *
   * Basically, we offer a report on the current state of leash.        *
   *                                                                    *
   ********************************************************************** */

static void leash_default(void)
{
   if (ships->leashed_to == (char)NIL) {
      c_out(LIGHTGREEN, "Your ship isn't leashed to any ship right now.\n\r");
   }
   else {
      if (Good_Hold(ships->leashed_to)) {
         if (hold[ships->leashed_to]->sxpos != 0 ||
               hold[ships->leashed_to]->sypos != 0) {
            c_out(LIGHTBLUE, "You are leashed to %s\n\r",
               hold[ships->leashed_to]->names);
            see_if_break_leash();
         }
         else {
            c_out(LIGHTBLUE, "The ship that you leashed to was destroyed\n\r");
            ships->leashed_to = (char)NIL;
            write_user();
         }
      }
      else {
         ships->leashed_to = (char)NIL;
         write_user();
      }
   }
}


