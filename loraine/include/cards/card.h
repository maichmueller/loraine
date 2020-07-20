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

   // the short ID, a running uint counting its card position in the total
   // card gallery
   const SID m_id;
   // the unique id used to identify this specific instance of a card
   UUID m_uuid;

   // variable attributes of the cards

   // the mana it costs to play the cards
   size_t m_mana_cost;
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
   [[nodiscard]] auto get_mana_cost() const { return m_mana_cost; }
   [[nodiscard]] auto get_keywords() const { return m_keywords; }
   [[nodiscard]] auto get_effects_map() const { return m_effects; }
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

   void set_mana_cost(size_t mana_cost) { m_mana_cost = mana_cost; }

   void set_keywords(std::vector< Keyword > keywords)
   {
      m_keywords = std::move(keywords);
   }
   void set_effect(events::EventType e_type, Effect effect)
   {
      m_effects[e_type] = Effect(std::move(effect));
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
      size_t mana_cost,
      std::initializer_list< Keyword > keyword_list,
      std::map< events::EventType, Effect > effects)
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
         m_mana_cost(mana_cost),
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
         m_mana_cost(card.get_mana_cost()),
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

   virtual bool play_condition_check() = 0;
};

class Unit: public Card {
  public:
   void set_damage(size_t damage) { m_damage = damage; }
   void set_health(size_t health) { m_health = health; }
   void set_flag_is_damaged(bool is_damaged) { m_is_damaged = is_damaged; }
   [[nodiscard]] auto get_damage() const { return m_damage; }
   [[nodiscard]] auto get_health() const { return m_health; }
   [[nodiscard]] auto is_damaged() const { return m_is_damaged; }

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
      size_t mana_cost,
      const std::initializer_list< Keyword >& keyword_list,
      std::map< events::EventType, Effect > effects,
      size_t damage,
      size_t health,
      bool is_damaged)
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
          mana_cost,
          keyword_list,
          std::move(effects)),
         m_damage(damage),
         m_health(health),
         m_is_damaged(is_damaged)
   {
   }
   Unit(const Unit& card)
       : Card(card),
         m_damage(card.get_damage()),
         m_health(card.get_health()),
         m_is_damaged(card.is_damaged())
   {
   }

  private:
   // unit cards based attributes

   // the damage the unit deals
   size_t m_damage;
   // the health of the unit
   size_t m_health;
   // whether the unit is currently damaged
   bool m_is_damaged;
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
      std::map< events::EventType, Effect > effects)
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
          std::move(effects))
   {
   }
};


// using VariantCard = std::variant <Follower, Champion, Spell>;

#endif  // LORAINE_CARD_H
