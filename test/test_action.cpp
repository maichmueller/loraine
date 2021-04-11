
#include "test_action.h"


TEST_F(ActionTest, move_unit)
{
   sptr< TestController > controller_blue = std::dynamic_pointer_cast< TestController >(
      state.player(Team::BLUE).controller());
   sptr< TestController > controller_red = std::dynamic_pointer_cast< TestController >(
      state.player(Team::RED).controller());

   state.logic()->draw_card(Team::BLUE);
   state.logic()->draw_card(Team::BLUE);
   state.logic()->draw_card(Team::BLUE);
   state.logic()->draw_card(Team::BLUE);

   state.logic()->draw_card(Team::RED);
   state.logic()->draw_card(Team::RED);
   state.logic()->draw_card(Team::RED);
   state.logic()->draw_card(Team::RED);

   controller_blue->add_action(actions::Action(
      actions::PlayAction(Team::BLUE, 0)));

   controller_blue->add_action(actions::Action(
      actions::PlayAction(Team::BLUE, 0)));

   controller_blue->add_action(actions::Action(
      actions::PlayAction(Team::BLUE,  0)));

   controller_blue->add_action(actions::Action(
      actions::PlayAction(Team::BLUE,  0)));


   controller_blue->add_action(actions::Action(actions::PlaceUnitAction(Team::BLUE, true, {0, 2})));

   auto& camp_blue = state.board().camp(Team::BLUE);
   auto& camp_red = state.board().camp(Team::RED);
}