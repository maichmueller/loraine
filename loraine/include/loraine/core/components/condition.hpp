
#ifndef LORAINE_CONDITION_HPP
#define LORAINE_CONDITION_HPP

class GameState;

/**
 * A std::function< bool(GameState&) > wrapper, which should be used to
 * determine whether a condition on the state is fulfilled;
 *
 * This can be used for cards when wanting to determine, whether it can be played.
 * @tparam Derived
 */
struct Condition {
   std::function< bool(GameState&) > func;
};

#endif  // LORAINE_CONDITION_HPP
