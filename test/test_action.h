

#ifndef LORAINE_TEST_ACTION_H
#define LORAINE_TEST_ACTION_H

#include <gtest/gtest.h>

#include <stack>

#include "test_cards.h"
#include "all.h"

struct TestController: public Controller {
   using Controller::Controller;

   std::stack< actions::Action > action_stack;

   actions::Action choose_action(const GameState& state) override
   {
      auto a = action_stack.top();
      action_stack.pop();
      return a;
   }
   actions::Action choose_targets(const GameState& state, const sptr< IEffect >& effect) override
   {
      auto a = action_stack.top();
      action_stack.pop();
      return a;
   }

   void add_action(const actions::Action& action) { action_stack.emplace(action); }
   void pop() { action_stack.pop(); }
};

class ActionTest: public ::testing::Test {
  protected:
   GameState state = init_state();
   std::invoke_result_t< decltype(&random::create_rng), int > rng = random::create_rng(0);

   GameState init_state()
   {
      Team t = BLUE;
      Deck deck_blue({
         std::make_shared< TestUnit1 >(t),
         std::make_shared< TestUnit1 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit4 >(t),
         std::make_shared< TestUnit4 >(t),
         std::make_shared< TestUnit5 >(t),
         std::make_shared< TestUnit5 >(t),
      });
      t = RED;
      Deck deck_red({
         std::make_shared< TestUnit1 >(t),
         std::make_shared< TestUnit1 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit2 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit3 >(t),
         std::make_shared< TestUnit4 >(t),
         std::make_shared< TestUnit4 >(t),
         std::make_shared< TestUnit5 >(t),
         std::make_shared< TestUnit5 >(t),
      });

      return GameState(
         Config(),
         {deck_blue, deck_red},
         {std::make_shared< TestController >(BLUE), std::make_shared< TestController >(RED)},
         rng);
   }
};

#endif  // LORAINE_TEST_ACTION_H
