
/* **********************************************************************
   * scout.h                                                            *
   *                                                                    *
   * Copyrite 1990, Fredric L. Rice. All rites reserved.                *
   *                                                                    *
   * This is the data structure for the scout ships.                    *
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

   extern struct scout_info *scouts[10];
   extern char next_scout_to_move;

