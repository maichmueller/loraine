
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <event/event.h>

#include <functional>
#include <utility>

// forward-declaration of state
class Game;
// forward-declaration of card
class Card;

class EffectContainer {
  public:
   using EffectFunc = std::function< void(Game&, const events::VariantEvent&) >;
   using ConditionFunc = std::function< bool(
      Game&, const events::VariantEvent&) >;

   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] bool is_registered() const { return m_is_registered; }
   [[nodiscard]] auto get_effect_func() const { return m_effect_func; }
   [[nodiscard]] auto get_condition_func() const { return m_condition_func; }
   [[nodiscard]] auto get_counter_round() const { return m_counter_rnd; }
   [[nodiscard]] auto get_counter_total() const { return m_counter_total; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   [[nodiscard]] auto get_owner() const { return m_owner; }
   void reset_counter_round() { m_counter_rnd = 0; }
   void set_assoc_card(sptr< Card > card_ptr)
   {
      m_assoc_card = std::move(card_ptr);
   }
   void set_effect(const EffectFunc& effect) { m_effect_func = effect; }
   void set_registered(bool flag) { m_is_registered = flag; }
   void operator()(Game& game, const events::VariantEvent& event)
   {
      m_effect_func(game, event);
      m_counter_rnd += 1;
      m_counter_total += 1;
   }
   [[nodiscard]] bool check_condition(
      Game& game, const events::VariantEvent& event)
   {
      return m_condition_func(game, event);
   }
   bool operator==(const EffectContainer& effect)
   {
      return m_is_null == effect.is_null()
             && m_is_registered == effect.is_registered()
             && m_counter_rnd == effect.get_counter_round()
             && m_counter_total == effect.get_counter_total()
             && _get_address(m_effect_func)
                   == _get_address(effect.get_effect_func());
   }

   ~EffectContainer() = default;

//   EffectContainer()
//       : m_effect_func([](Game& state, const events::VariantEvent& event) {}),
//         m_owner(PLAYER::BLUE)
//   {
//   }  // empty effect machine default constructor (only used for placeholders)

   explicit EffectContainer(PLAYER owner)
       : m_effect_func([](Game& state, const events::VariantEvent& event) {}),
         m_owner(owner)
   {
   }  // empty effect machine

   EffectContainer(
      PLAYER owner, EffectFunc effect_func, ConditionFunc condition_func)
       : m_effect_func(std::move(effect_func)),
         m_condition_func(std::move(condition_func)),
         m_is_null(false),
         m_owner(owner)
   {
   }
   EffectContainer(
      PLAYER owner,
      EffectFunc effect_func,
      ConditionFunc condition_func,
      sptr< Card > card_ptr)
       : m_effect_func(std::move(effect_func)),
         m_condition_func(std::move(condition_func)),
         m_is_null(false),
         m_assoc_card(std::move(card_ptr)),
         m_owner(owner)
   {
   }

   EffectContainer(const EffectContainer& effect)
       : m_effect_func(effect.get_effect_func()),
         m_condition_func(effect.get_condition_func()),
         m_is_null(effect.is_null()),
         m_is_registered(effect.is_registered()),
         m_counter_rnd(effect.get_counter_round()),
         m_counter_total(effect.get_counter_total()),
         m_assoc_card(effect.get_associated_card()),
         m_owner(effect.get_owner())
   {
   }
   EffectContainer(EffectContainer&& effect) noexcept
   {
      m_effect_func = std::move(effect.m_effect_func);
      m_condition_func = std::move(effect.m_condition_func);

      m_is_null = effect.m_is_null;
      m_is_registered = effect.m_is_registered;
      m_counter_rnd = effect.m_counter_rnd;
      m_counter_total = effect.m_counter_total;

      m_assoc_card = effect.m_assoc_card;
      effect.m_assoc_card = nullptr;

      m_owner = effect.m_owner;
   }

   EffectContainer& operator=(EffectContainer&& effect) noexcept
   {
      if(*this == effect) {
         return *this;
      }
      m_effect_func = std::move(effect.m_effect_func);
      m_condition_func = std::move(effect.m_condition_func);

      m_is_null = effect.m_is_null;
      m_is_registered = effect.m_is_registered;
      m_counter_rnd = effect.m_counter_rnd;
      m_counter_total = effect.m_counter_total;

      m_assoc_card = effect.m_assoc_card;
      effect.m_assoc_card = nullptr;
      return *this;
   }

  private:
   EffectFunc m_effect_func;
   ConditionFunc m_condition_func;
   bool m_is_null{true};
   bool m_is_registered{false};
   size_t m_counter_rnd{0};
   size_t m_counter_total{0};
   sptr< Card > m_assoc_card{};
   PLAYER m_owner;

   /*
    * Get the address of the function pointer.
    *
    * Needed for effect function comparison of equality.
    *
    * this may need some further checkups for portability, as conversion to
    * size_t of a function pointer is implementation dependent.
    */
   template < typename ReturnType, typename... Params >
   size_t _get_address(std::function< ReturnType(Params...) > f)
   {
      using FuncType = ReturnType(Params...);
      auto** fn_ptr = f.template target< FuncType* >();
      return (size_t) *fn_ptr;
   }
};

#endif  // LORAINE_EFFECT_H
