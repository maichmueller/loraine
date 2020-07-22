
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <event/event.h>

#include <functional>
#include <utility>

// forward-declaration of state
class State;
// forward-declaration of card
class Card;

class Effect {
  public:
   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] bool is_registered() const { return m_is_registered; }
   [[nodiscard]] auto get_effect() const { return m_effect; }
   [[nodiscard]] auto get_counter_round() const { return m_counter_rnd; }
   [[nodiscard]] auto get_counter_total() const { return m_counter_total; }
   [[nodiscard]] auto get_associated_card() const { return m_assoc_card; }
   void reset_counter_round() { m_counter_rnd = 0; }
   void set_assoc_card(sptr< Card > card_ptr)
   {
      m_assoc_card = std::move(card_ptr);
   }
   void set_effect(
      const std::function< void(State&, const events::VariantEvent&) >& effect)
   {
      m_effect = effect;
   }
   void set_registered(bool flag) { m_is_registered = flag; }
   void operator()(State& state, const events::VariantEvent& event)
   {
      m_effect(state, event);
      m_counter_rnd += 1;
      m_counter_total += 1;
   }
   bool operator==(const Effect& effect)
   {
      return m_is_null == effect.is_null()
             && m_is_registered == effect.is_registered()
             && m_counter_rnd == effect.get_counter_round()
             && m_counter_total == effect.get_counter_total()
             && _get_address(m_effect) == _get_address(effect.get_effect());
   }

   Effect() : m_effect([](State& state, const events::VariantEvent& event) {})
   {
   }  // empty effect machine
   explicit Effect(
      std::function< void(State&, const events::VariantEvent&) > effect)
       : m_effect(std::move(effect)), m_is_null(false)
   {
   }
   Effect(
      std::function< void(State&, const events::VariantEvent&) > effect,
      sptr< Card > card_ptr)
       : m_effect(std::move(effect)),
         m_is_null(false),
         m_assoc_card(std::move(card_ptr))
   {
   }
   Effect(const Effect& effect)
       : m_effect(effect.get_effect()),
         m_is_null(effect.is_null()),
         m_is_registered(effect.is_registered()),
         m_counter_rnd(effect.get_counter_round()),
         m_counter_total(effect.get_counter_total()),
         m_assoc_card(effect.get_associated_card())
   {
   }
   Effect(Effect&& effect) noexcept
   {
      m_effect = effect.m_effect;
      effect.m_effect = nullptr;

      m_is_null = effect.m_is_null;
      m_is_registered = effect.m_is_registered;
      m_counter_rnd = effect.m_counter_rnd;
      m_counter_total = effect.m_counter_total;

      m_assoc_card = effect.m_assoc_card;
      effect.m_assoc_card = nullptr;
   }

   Effect& operator=(Effect&& effect) noexcept
   {
      if(*this == effect) {
         return *this;
      }
      m_effect = effect.m_effect;
      effect.m_effect = nullptr;

      m_is_null = effect.m_is_null;
      m_is_registered = effect.m_is_registered;
      m_counter_rnd = effect.m_counter_rnd;
      m_counter_total = effect.m_counter_total;

      m_assoc_card = effect.m_assoc_card;
      effect.m_assoc_card = nullptr;
      return *this;
   }

  private:
   std::function< void(State&, const events::VariantEvent&) > m_effect;
   bool m_is_null{true};
   bool m_is_registered{false};
   uint16_t m_counter_rnd{0};
   uint16_t m_counter_total{0};
   sptr< Card > m_assoc_card{};

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
