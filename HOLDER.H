
/* **********************************************************************
   * This is the structure for the in-memory values that are used to	*
   * acquire speed.                                                     *
   *                                                                    *
   ********************************************************************** */

struct holder {                     /* Holds player infomation in memory    */
   long sxpos, sypos;               /* Ships positions                      */
   UC szpos;                        /* Ships universe                       */
   long bxpos, bypos;               /* Base positions                       */
   UC bzpos;                        /* Base universe                        */
   short xremotes[10], yremotes[10];/* Ships remotes positions              */
   UC remote_universe[10];          /* What universe is it in?              */
   char names[5];                   /* Names of the ships                   */
   short standings;                 /* Standing information of the ships    */
   short kills;                     /* Number of kills for this ship        */
   short xswarm[5], yswarm[5];      /* Swarm x and y positions              */
   UC swarm_universe[5];            /* The universe the swarm is in.        */
   unsigned is_friendly : 1;        /* TRUE if peace, else FALSE.           */
} ;

extern struct holder *hold[TOTAL_PLAYERS];

