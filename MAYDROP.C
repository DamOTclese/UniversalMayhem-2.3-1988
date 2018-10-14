/* **********************************************************************
   * MayDrop.C - Copyright Fredric L. Rice, 1991. All rights reserved.  *
   *                                                                    *
   * This module scans various formats of drop files and extracts the   *
   * information Mayhem needs from it.                                  *
   *                                                                    *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *
   *                    DORINFO?.DEF file format                        *
   *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                                                    *
   * Item 1) The Hornet's Nest              ;BBS Name                   *
   * Item 2) Kato                           ;SysOp's First Name         *
   * Item 3) .                              ;SysOp's Last Name          *
   * Item 4) COM2                           ;Com Port (0 local mode)    *
   * Item 5) 19200 BAUD,N,8,1               ;Baud Rate (0 local mode)   *
   * Item 6) 0                              ;don't know/not important   *
   * Item 7) ROB                            ;User's First Name          *
   * Item 8) HALL                           ;User's Last Name           *
   * Item 9) Colorado Springs Co            ;User's Address (City,ST)   *
   * Item 10) 1                             ;Graphics (0 no graphics)   *
   * Item 11) 50                            ;User's Security level;     *
   * Item 12) 4                             ;Minutes remaining          *
   *                                                                    *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *
   *                     DOOR.SYS File format                           *
   *  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                                                    *
   * Item  1) COM0:                    ;ComPort (0 for local)           *
   * Item  2) 19200                    ;Baud Rate                       *
   * Item  3) 8                        ;Word Length                     *
   * Item  4) 1                        ;Stop Bits                       *
   * Item  5) 19200                    ;Baud locked at (0 if unlocked)  *
   * Item  6) Y                                                         *
   * Item  7) N                                                         *
   * Item  8) N                                                         *
   * Item  9) N                                                         *
   * Item 10) KATO .                   ;User Name (first and last)      *
   * Item 11) Tampa, Fl 33625          ;User City                       *
   * Item 12) 813-968-9103             ;User Phone;                     *
   * Item 13) 813-968-9103                                              *
   * Item 14) NOTREALLYIT              ;User Password                   *
   * Item 15) 255                      ;User Security level             *
   * Item 16) 522                                                       *
   * Item 17) 02/18/91                 ;User last on date?              *
   * Item 18) 14306                    ;User time left in seconds       *
   * Item 19) 238                      ;User Time left in minutes       *
   * Item 20) GR                       ;Graphics (I think none is NO)   *
   * Item 21) 24                                                        *
   * Item 22) N                                                         *
   * Item 23) 1234567                                                   *
   * Item 24) 1                                                         *
   * Item 25) 01/01/99                 ;User expiration date...         *
   * Item 26) 1                                                         *
   * Item 27) X                                                         *
   * Item 28) 0                                                         *
   * Item 29) 0                                                         *
   * Item 30) 0                                                         *
   * Item 31) 999999                                                    *
   *                                                                    *
   ********************************************************************** */

#include "stdio.h"
#include "defines.h"
#include "ship.h"
#include "dir.h"
#include "stdlib.h"

/* **********************************************************************
   * What data are we interested in?                                    *
   *                                                                    *
   ********************************************************************** */

    extern char *record;
    extern short drop_time_remaining;

/* **********************************************************************
   * Get the line of data out of the offered line and return the        *
   * string or the numeric in the address offered as well.              *
   *                                                                    *
   * Return TRUE if the read was performed, else return FALSE.          *
   *                                                                    *
   ********************************************************************** */

static char get_line_data(FILE *this_file, char this_element)
{
    char line_count;

    rewind(this_file);

    for (line_count = 0; line_count < this_element; line_count++) {
        if (! feof(this_file)) {
            fgets(record, 100, this_file);
        }
        else {
            return(FALSE);
        }
    }

    if (feof(this_file)) {
        return(FALSE);
    }

    return(TRUE);
}

/* **********************************************************************
   * Scan for these types of files:                                     *
   *                                                                    *
   *            DORINFO?.DEF                                            *
   *            DOOR.SYS                                                *
   *                                                                    *
   * Extract the following information:                                 *
   *                                                                    *
   *            First Name                                              *
   *            Last Name                                               *
   *            Time remaining on BBS                                   *
   *                                                                    *
   * Return TRUE if any information was found, else return FALSE.       *
   *                                                                    *
   ********************************************************************** */

char extract_drop_information(void)
{
    FILE *door_file;
    struct ffblk file_block;
    char result;
    char hold_name[101], *atpoint, c_count;

/*
    We look for DORINFO?.DEF type files first. We only accept the first
    type of this file we encounter.
*/

    result = findfirst("DORINFO?.DEF", &file_block, 0x16);

    if (! result) {
        if ((door_file = fopen(file_block.ff_name, "rt")) != (FILE *)NULL) {
            if (! get_line_data(door_file, 7)) {
                log_error(145);
                return(FALSE);
            }

            atpoint = record;
            c_count = 0;
            skipspace(atpoint);
            while (*atpoint && *atpoint != ' ') {
                hold_name[c_count] = *atpoint;
                c_count++;
                hold_name[c_count] = (char)NULL;
                atpoint++;
            }

            if (! get_line_data(door_file, 8)) {
                log_error(145);
                return(FALSE);
            }

            hold_name[c_count++] = ' ';

            atpoint = record;
            skipspace(atpoint);
            while (*atpoint && *atpoint != ' ') {
                hold_name[c_count] = *atpoint;
                c_count++;
                hold_name[c_count] = (char)NULL;
                atpoint++;
            }

            if (! get_line_data(door_file, 12)) {
                log_error(145);
                return(FALSE);
            }

            drop_time_remaining = atoi(record);
            strcpy(record, hold_name);
            return(TRUE);
        }
        else {
            log_error(146);
        }
    }

/*
    That file wasn't found. Now look for DOOR.SYS
*/

    result = findfirst("DOOR.SYS", &file_block, 0x16);

    if (! result) {
        if ((door_file = fopen(file_block.ff_name, "rt")) != (FILE *)NULL) {
            if (! get_line_data(door_file, 10)) {
                log_error(145);
                return(FALSE);
            }

            atpoint = record;
            skipspace(atpoint);
            strncpy(hold_name, atpoint, 30);

            if (! get_line_data(door_file, 19)) {
                log_error(145);
                return(FALSE);
            }

            drop_time_remaining = atoi(record);
            strcpy(record, hold_name);
            return(TRUE);
        }
        else {
            log_error(146);
        }
    }

/*
    Neither type of file was found. Issue a warning.
*/

    log_error(147);
    return(FALSE);
}


