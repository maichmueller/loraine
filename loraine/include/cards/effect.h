
#ifndef LORAINE_EFFECT_H
#define LORAINE_EFFECT_H

#include <event.h>

#include <functional>

// forward-declaration of state
class State;

class Effect {
  public:
   [[nodiscard]] bool is_null() const { return m_is_null; }
   [[nodiscard]] auto get_effect() { return m_effect; }
   void reset_counter_round() { m_counter_rnd = 0; }
   void set_card_uuid(UUID uuid) { m_assoc_card_id = uuid; }
   void set_effect(
      const std::function< void(State&, const events::VariantEvent&) >& effect)
   {
      m_effect = effect;
   }
   void operator()(State& state, const events::VariantEvent& event)
   {
      m_effect(state, event);
      m_counter_rnd += 1;
      m_counter_total += 1;
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
      UUID uuid)
       : m_effect(std::move(effect)), m_is_null(false), m_assoc_card_id(uuid)
   {
   }

  private:
   std::function< void(State&, const events::VariantEvent&) > m_effect;
   bool m_is_null{true};
   uint16_t m_counter_rnd{0};
   uint16_t m_counter_total{0};
   UUID m_assoc_card_id{};
};

#endif  // LORAINE_EFFECT_H
