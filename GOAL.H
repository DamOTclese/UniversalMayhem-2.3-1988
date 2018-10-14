
/* **********************************************************************
   * So that we may find the items easily and know if they are aboard a	*
   * ship, we store the location and a flag in memory.			*
   *									*
   * If 'goal_on_ship' is NIL, then the item is drifting in space. If	*
   * it's anything else, then it's aboard a ship.			*
   *									*
   ********************************************************************** */

   struct goal_elements {
      short goal_xpos;
      short goal_ypos;
      char goal_on_ship;
      UC goal_universe;		/* What universe number is it in	*/
   } ;

   extern struct goal_elements *goal_item[10], goals;

