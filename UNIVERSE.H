
/* **********************************************************************
   * This is the structure of the UNIVERSE.DAT data file.               *
   *                                                                    *
   * It contains everything about the universe we need to know.         *
   *                                                                    *
   ********************************************************************** */

struct universe_file {     /* Structure of UNIVERSE.DAT file:           */
   short planets[4];       /* Yposes of four planets in a record        */
   short star;             /* Ypos of a star on this record of universe */
   short mine;             /* Ypos of a mine on this record of universe */
   short black_hole;       /* Ypos of black hole on this record         */
   short white_hole;       /* Exit xpoint position for blank hole       */
} ;

extern struct universe_file universe;

