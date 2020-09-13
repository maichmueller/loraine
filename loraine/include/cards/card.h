//
// Created by michael on 29.05.20.
//

#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <map>
#include <utility>
#include <vector>

#include "effect.h"
#include "event/event.h"
#include "keywords.h"
#include "region.h"
#include "types.h"
#include "uuid_gen.h"

// forward-declaration of state
class State;

enum struct Rarity { NONE, COMMON, RARE, EPIC, CHAMPION };

enum struct Group {
   NONE,
   ELITE,
   ELNUK,
   PORO,
   SEA_MONSTER,
   SPIDER,
   TECH,
   TREASURE,
   YETI,
};

enum struct CardType { SPELL, UNIT, SKILL };

enum struct UnitType { NONE, FOLLOWER, CHAMPION };

/*
 * Abstract base class (abc) for LOR cards.
 */
class Card {
   // effects need to have access to members to compute their conditions
   friend Effect;

  protected:
   // fixed attributes of the cards

   // the cards's name
   const char* const m_name;
   // the description text of the cards
   const char* const m_effect_desc;
   // text of the cards in context of the lol universe
   const char* const m_lore;
   // the region the cards is from
   const Region m_region;
   // the subgroup the cards belongs to
   const Group m_group;
   // the unit type (champion, follower or spell/skill=none)
   const UnitType m_unit_type;
   // the rarity of the cards
   const Rarity m_rarity;
   // whether the cards is a spell or a unit
   const CardType m_card_type;
   // whether a card is collectible (i.e. can be added to a deck)
   const bool m_is_collectible;

   // the short ID, a running uint counting its index in the total card gallery
   const SID m_id;
   // the unique id used to identify this specific instance of a card
   UUID m_uuid;

   // variable attributes of the cards

   // the mana it costs to play the cards (as default)
   const size_t m_mana_cost_base;
   // the current change to the mana cost of the card
   int m_mana_cost_delta;
   // all the keywords pertaining to the cards
   std::vector< Keyword > m_keywords;

   // all possible effects
   std::map< events::EventType, Effect > m_effects;

  public:
   void operator()(
      State& state,
      const events::VariantEvent& event,
      events::EventType event_type)
   {
      m_effects[event_type](state, event);
   }
   // convenience operator of the above
   void operator()(State& state, const events::VariantEvent& event)
   {
      events::EventType eve_type = std::visit(
         events::VisitorEventType{}, event);
      (*this)(state, event, eve_type);
   }

   [[nodiscard]] auto get_name() const { return m_name; }
   [[nodiscard]] auto get_effect_desc() const { return m_effect_desc; }
   [[nodiscard]] auto get_lore() const { return m_lore; }
   [[nodiscard]] auto get_region() const { return m_region; }
   [[nodiscard]] auto get_group() const { return m_group; }
   [[nodiscard]] auto get_unit_type() const { return m_unit_type; }
   [[nodiscard]] auto get_rarity() const { return m_rarity; }
   [[nodiscard]] auto get_card_type() const { return m_card_type; }
   [[nodiscard]] auto get_mana_cost_base() const { return m_mana_cost_base; }
   [[nodiscard]] auto get_mana_cost_delta() const { return m_mana_cost_delta; }
   [[nodiscard]] auto get_mana_cost() const
   {
      return m_mana_cost_base + m_mana_cost_delta;
   }
   [[nodiscard]] const auto& get_keywords() const { return m_keywords; }
   [[nodiscard]] const auto& get_effects_map() const { return m_effects; }
   [[nodiscard]] auto get_id() const { return m_id; }
   [[nodiscard]] auto get_uuid() const { return m_uuid; }

   // status requests
   [[nodiscard]] auto is_collectible() const { return m_is_collectible; }
   [[nodiscard]] auto is_unit() const { return m_card_type == CardType::UNIT; }
   [[nodiscard]] auto is_spell() const
   {
      return m_card_type == CardType::SPELL;
   }
   [[nodiscard]] auto is_skill() const
   {
      return m_card_type == CardType::SKILL;
   }
   [[nodiscard]] auto is_champion() const
   {
      return m_unit_type == UnitType::CHAMPION;
   }
   [[nodiscard]] auto is_follower() const
   {
      return m_unit_type == UnitType::FOLLOWER;
   }

   void set_mana_cost(size_t mana_cost)
   {
      m_mana_cost_delta = mana_cost - m_mana_cost_base;
   }

   void set_keywords(std::vector< Keyword > keywords)
   {
      m_keywords = std::move(keywords);
   }
   void set_effect(events::EventType e_type, Effect effect)
   {
      m_effects[e_type] = std::move(effect);
   }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const
   {
      return std::find(m_keywords.begin(), m_keywords.end(), kword)
             != m_keywords.end();
   }

   /*
    * A defaulted virtual destructor needed bc of inheritance
    */
   virtual ~Card() = default;

   /*
    *  Basic constructor
    */
   Card(
      SID id,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      UnitType unit_type,
      Rarity rarity,
      CardType card_type,
      bool is_collectible,
      size_t mana_cost_base,
      std::initializer_list< Keyword > keyword_list,
      std::map< events::EventType, Effect > effects,
      int mana_cost_delta = 0)
       : m_name(name),
         m_effect_desc(effect_desc),
         m_lore(lore),
         m_region(region),
         m_group(group),
         m_unit_type(unit_type),
         m_rarity(rarity),
         m_card_type(card_type),
         m_is_collectible(is_collectible),
         m_id(id),
         m_uuid(new_uuid()),
         m_mana_cost_base(mana_cost_base),
         m_mana_cost_delta(mana_cost_delta),
         m_keywords(keyword_list),
         m_effects(std::move(effects))
   {
   }

   /*
    * Copy Constructor
    */
   Card(const Card& card)
       : m_name(card.get_name()),
         m_effect_desc(card.get_effect_desc()),
         m_lore(card.get_lore()),
         m_region(card.get_region()),
         m_group(card.get_group()),
         m_unit_type(card.get_unit_type()),
         m_rarity(card.get_rarity()),
         m_card_type(card.get_card_type()),
         m_is_collectible(card.is_collectible()),
         m_id(card.get_id()),
         m_uuid(new_uuid()),
         m_mana_cost_base(card.get_mana_cost_base()),
         m_mana_cost_delta(card.get_mana_cost_delta()),
         m_keywords(card.get_keywords()),
         m_effects(card.get_effects_map())
   {
   }

   /*
    * Deleted copy assignment operator
    */
   Card& operator=(const Card&) = delete;

   /*
    * Move assignment operator
    */
   Card& operator=(Card&&) = delete;

   /*
    * Move constructor.
    */
   Card(Card&& card) = delete;

   virtual bool play_condition_check() { return true; }

   void add_mana_cost(int amount) { m_mana_cost_delta += amount; }
};

class Unit: public Card {
  private:
   // unit cards based attributes
   bool m_alive = true;

   // the base damage the unit deals.
   const size_t m_power_base = 0;
   // the base health of the unit
   const size_t m_health_base = 0;

   // The current change in health and damage attributes (hence 'delta')

   // the damage change the unit deals.
   int m_power_delta;
   // the health change of the unit
   int m_health_delta;

   // whether the unit is currently damaged
   bool m_damaged = false;

  public:
   inline void set_power(size_t power)
   {
      m_power_delta = static_cast< decltype(m_power_delta) >(
         power - m_power_base);
   }
   inline void set_health(size_t health)
   {
      m_health_delta = static_cast< decltype(m_health_delta) >(
         health - m_health_base);
   }
   [[noreturn]] inline void set_flag_damaged(bool damaged)
   {
      m_damaged = damaged;
   }
   [[noreturn]] inline void set_flag_alive(bool alive)
   {
      m_alive = alive;
   }
   [[nodiscard]] inline auto get_power_base() const { return m_power_base; }
   [[nodiscard]] inline auto get_power_delta() const { return m_power_delta; }
   [[nodiscard]] inline auto get_power() const
   {
      return m_power_base + m_power_delta;
   }
   [[nodiscard]] inline auto get_health_base() const { return m_health_base; }
   [[nodiscard]] inline auto get_health_delta() const { return m_health_delta; }
   [[nodiscard]] inline auto get_health() const
   {
      return m_health_base + m_health_delta;
   }

   [[nodiscard]] inline bool is_alive() const {
      return m_alive;
   }

   inline int add_health(int amount)
   {
      m_health_delta = amount;
      return get_health();
   }

   Unit(
      SID id,
      char* const name,
      char* const effect_desc,
      char* const lore,
      Region region,
      Group group,
      UnitType unit_type,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost_base,
      size_t power_base,
      size_t health_base,
      const std::initializer_list< Keyword >& keyword_list,
      std::map< events::EventType, Effect > effects,
      int mana_cost_delta = 0,
      int power_delta = 0,
      int health_delta = 0,
      bool is_damaged = false
      //      std::function< bool(const std::optional< sptr< Card > >&, size_t)
      //      >
      //         check_is_damageable =
      //            [](const std::optional< sptr< Card > >& /*unused*/,
      //               size_t /*unused*/) { return true; },
      //      std::function< bool(const std::optional< sptr< Card > >&) >
      //         check_is_killable =
      //            [](const std::optional< sptr< Card > >& /*unused*/) {
      //               return true;
      //            })
      )
       : Card(
          id,
          name,
          effect_desc,
          lore,
          region,
          group,
          unit_type,
          rarity,
          CardType::UNIT,
          is_collectible,
          mana_cost_base,
          keyword_list,
          std::move(effects),
          mana_cost_delta),
         m_power_base(power_base),
         m_health_base(health_base),
         m_power_delta(power_delta),
         m_health_delta(health_delta),
         m_damaged(is_damaged)
   //         m_check_is_damageable(std::move(check_is_damageable)),
   //         m_check_is_killable(std::move(check_is_killable))
   {
   }
   Unit(const Unit& card)
       : Card(card),
         m_power_delta(card.get_power_delta()),
         m_health_delta(card.get_health_delta())
   //         m_is_damaged(card.is_damaged()),
   //         m_check_is_damageable(std::move(card.get_check_is_damageable()))
   {
   }
};

class Spell: public Card {
   Spell(
      SID id,
      char* const name,
      char* const effect_desc,
      char* const lore,
      Region region,
      Group group,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost,
      const std::initializer_list< Keyword >& keyword_list,
      std::map< events::EventType, Effect > effects,
      int mana_cost_delta = 0)
       : Card(
          id,
          name,
          effect_desc,
          lore,
          region,
          group,
          UnitType::NONE,
          rarity,
          CardType::SPELL,
          is_collectible,
          mana_cost,
          keyword_list,
          std::move(effects),
          mana_cost_delta)
   {
   }
};

// using VariantCard = std::variant <Follower, Champion, Spell>;

#endif  // LORAINE_CARD_H
