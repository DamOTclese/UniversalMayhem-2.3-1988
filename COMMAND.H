
/* **********************************************************************
   * The structure which contains all of the valid information which	*
   * is contained within the *.SHP command files.                       *
   *                                                                    *
   ********************************************************************** */

struct command_options {        /* Command ship automation information  */
   short ship;                  /* The ships number                     */
   unsigned assist : 1;         /* Does this ship wish to assist?       */
   unsigned defend : 1;         /* Does this ship wish to defend        */
   unsigned attack : 1;         /* Does this ship want to attack?       */
   unsigned run_from : 1;       /* Does this ship want to run from?     */
   unsigned decoy : 1;          /* Does this ship employ a decoy?       */
   unsigned bail_1_out : 1;     /* Does it whis to bail out level 1?    */
   unsigned bail_2_out : 1;     /* Does it whis to bail out level 2?    */
   unsigned bail_3_out : 1;     /* Does it wish to bail out level 3?    */
   unsigned bail_4_out : 1;     /* Does it wish to bail out level 4?    */
   unsigned default_fight : 1;  /* 0-Don't fight, 1-Fight               */
   short decoy_class;           /* What class shuttle for decoy         */
   UL bail_1_value;             /* Bail out value 1                     */
   short bail_2_value;          /* Bail out value 2                     */
   short bail_3_value;          /* Bail out value 3                     */
   UL phaser;                   /* How much phaser power to use?        */
   struct command_options *next; /* Linked list pointer to next         */
} ;

extern struct command_options multi;
extern struct command_options *m_first, *m_last, *m_point, *m_next;
extern struct command_options *m_test, *m_run, *m_previous;

