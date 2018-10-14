
/* **********************************************************************
   * This is the structure for the COMETS.DAT data file.		*
   *									*
   ********************************************************************** */

   extern struct comets_file {  /* Structure of COMETS.SHP                */
      unsigned flag : 1;        /* Set TRUE if scanned before else FALSE  */
      unsigned direction : 1;   /* The orbital direction of the comet     */
      long location[2];         /* The x pos and y pos of the comet       */
      char name[15];            /* Name of comet names by first scanned   */
      char ship[5];             /* Name of the ship who names the comet   */
   } *comets[40];

