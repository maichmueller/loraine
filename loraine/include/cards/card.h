//
// Created by michael on 29.05.20.
//

#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <utility>
#include <vector>

#include "effect.h"
#include "event.h"
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

enum struct CardType { SPELL, UNIT };

enum struct UnitType { NONE, FOLLOWER, CHAMPION };

/*
 * Abstract base class (abc) for LOR cards.
 */
class Card {
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
   const CardType m_cardtype;

   // the short ID, a running uint counting its card position in the total
   // card gallery
   const SID m_id;
   // the unique id used to identify this specific instance of a card
   UUID m_uuid;

   // variable attributes of the cards

   // the mana it costs to play the cards
   u64 m_mana_cost;
   // all the keywords pertaining to the cards
   std::vector< Keyword > m_keywords;

   // unit cards based attributes

   // the damage the unit deals
   u64 m_damage;
   // the health of the unit
   u64 m_health;

  private:
   // all possible effects
   std::map< events::EventType, Effect > m_effects;

  public:
   virtual ~Card() = default;
   // the general constructor for initializing the members
   Card(
      SID id,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      UnitType unit_type,
      Rarity rarity,
      CardType cardtype,
      u64 mana_cost,
      u64 damage,
      u64 health,
      const std::initializer_list< Keyword >& kword_list)
       : m_name(name),
         m_effect_desc(effect_desc),
         m_lore(lore),
         m_region(region),
         m_group(group),
         m_unit_type(unit_type),
         m_rarity(rarity),
         m_cardtype(cardtype),
         m_id(id),
         m_uuid(new_uuid()),
         m_mana_cost(mana_cost),
         m_keywords(kword_list),
         m_damage(damage),
         m_health(health)
   {
   }

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
   [[nodiscard]] auto get_cardtype() const { return m_cardtype; }
   [[nodiscard]] auto get_mana_cost() const { return m_mana_cost; }
   [[nodiscard]] auto get_damage() const { return m_damage; }
   [[nodiscard]] auto get_health() const { return m_health; }
   [[nodiscard]] auto get_keywords() const { return m_keywords; }
   [[nodiscard]] auto get_effects() const { return m_effects; }

   void set_mana_cost(u64 mana_cost) { m_mana_cost = mana_cost; }
   void set_damage(u64 damage) { m_damage = damage; }
   void set_health(u64 health) { m_health = health; }
   void set_keywords(std::vector< Keyword > kwords)
   {
      m_keywords = std::move(kwords);
   }
   void set_effect(
      events::EventType e_type,
      std::function< void(State&, const events::VariantEvent&) > effect)
   {
      m_effects[e_type] = Effect(std::move(effect), m_uuid);
   }
};

class Unit: public Card {
   Unit(
      SID id,
      char* const name,
      char* const effect_desc,
      char* const lore,
      Region region,
      Group group,
      UnitType unit_type,
      Rarity rarity,
      u64 mana_cost,
      u64 damage,
      u64 health,
      const std::initializer_list< Keyword >& kword_list)
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
          mana_cost,
          damage,
          health,
          kword_list)
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
      Rarity rarity,
      u64 mana_cost,
      const std::initializer_list< Keyword >& kword_list)
       : Card(
          id,
          name,
          effect_desc,
          lore,
          region,
          Group::NONE,
          UnitType::NONE,
          rarity,
          CardType::SPELL,
          mana_cost,
          0,
          0,
          kword_list)
   {
   }
};

#endif  // LORAINE_CARD_H
