
/* **********************************************************************
   * This is the structure of the PLANETS.DAT data file.                *
   *                                                                    *
   * It contains everything we need to know about the planets.          *
   *                                                                    *
   * The value 'named[]' MUST be the last value in this structure as 	*
   * the SysOp has the option of turning on or off the nameing of       *
   * planets to save disk space.                                        *
   *                                                                    *
   ********************************************************************** */

struct planets_file {           /* Structure of PLANETS.DAT file:           */
   unsigned char cost;          /* Cost factor used to determine costs      */
   unsigned char technology;    /* Technology level of the planet           */
   char visited;                /* Last visited by ship name                */
   char protected;              /* Planet protected by ship name            */
   char plagued;                /* Planet plagued by ship name              */
   char named[16];              /* Planets name (placed by a player)        */
} ;

extern struct planets_file planets;

