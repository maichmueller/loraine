//
// Created by michael on 29.05.20.
//

#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "effect.h"
#include "event/event.h"
#include "keywords.h"
#include "region.h"
#include "types.h"
#include "uuid_gen.h"

// forward-declarations
class State;
class Grant;
class Game;

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
   friend EffectContainer;

  private:
   // fixed attributes of the cards

   // the cards' name
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
   const size_t m_mana_cost_ref;
   // when effects move the base cost to a new permanent value
   long int m_mana_cost_base;
   // the current change to the mana cost of the card
   long int m_mana_cost_delta;
   // all the keywords pertaining to the cards
   KeywordMap m_keywords;

   // all possible effects
   std::map< events::EventType, EffectContainer > m_effects;
   // all grants bestowed on the card
   std::map< UUID, sptr< Grant > > m_grants;

   // the player whose card this is
   PLAYER m_owner;

  public:
   inline void operator()(
      Game& game,
      const events::VariantEvent& event,
      events::EventType event_type)
   {
      m_effects.at(event_type)(game, event);
   }
   // convenience operator of the above
   inline void operator()(Game& game, const events::VariantEvent& event)
   {
      (*this)(game, event, get_event_type(event));
   }

   [[nodiscard]] auto get_name() const { return m_name; }
   [[nodiscard]] auto get_effect_desc() const { return m_effect_desc; }
   [[nodiscard]] auto get_lore() const { return m_lore; }
   [[nodiscard]] auto get_region() const { return m_region; }
   [[nodiscard]] auto get_group() const { return m_group; }
   [[nodiscard]] auto get_unit_type() const { return m_unit_type; }
   [[nodiscard]] auto get_rarity() const { return m_rarity; }
   [[nodiscard]] auto get_card_type() const { return m_card_type; }
   [[nodiscard]] auto get_mana_cost_ref() const { return m_mana_cost_ref; }
   [[nodiscard]] auto get_mana_cost_base() const { return m_mana_cost_base; }
   [[nodiscard]] auto get_mana_cost_delta() const { return m_mana_cost_delta; }
   [[nodiscard]] auto get_mana_cost() const
   {
      return std::max(0L, m_mana_cost_base + m_mana_cost_delta);
   }
   [[nodiscard]] const auto& get_keywords() const { return m_keywords; }
   [[nodiscard]] auto& get_keywords() { return m_keywords; }
   [[nodiscard]] const auto& get_effects_map() const { return m_effects; }
   [[nodiscard]] const auto& get_grants_map() const { return m_grants; }
   [[nodiscard]] auto& get_grants_map() { return m_grants; }
   [[nodiscard]] auto get_id() const { return m_id; }
   [[nodiscard]] auto get_uuid() const { return m_uuid; }
   [[nodiscard]] PLAYER get_owner() const { return m_owner; }

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

   void reduce_mana_cost(long int amount) { m_mana_cost_delta += amount; }
   void set_mana_cost(size_t mana_cost)
   {
      m_mana_cost_delta = static_cast< long int >(mana_cost - m_mana_cost_base);
   }

   void set_keywords(KeywordMap keywords) { m_keywords = keywords; }

   void set_effect(events::EventType e_type, EffectContainer effect)
   {
      // if the key is already found in the effects map, delete the previous
      // effect. This essentially implies we overwrite preexisting effects
      if(m_effects.find(e_type) != m_effects.end()) {
         m_effects.erase(e_type);
      }
      m_effects.emplace(e_type, std::move(effect));
   }

   inline void remove_effect(events::EventType e_type)
   {
      m_effects.erase(e_type);
   }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const
   {
      return m_keywords.at(static_cast< unsigned long >(kword));
   }
   inline void add_keyword(Keyword kword)
   {
      m_keywords[static_cast< unsigned long >(kword)] = true;
   }
   inline void remove_keyword(Keyword kword)
   {
      m_keywords[static_cast< unsigned long >(kword)] = false;
   }

   virtual bool play_condition_check() { return true; }
   /*
    * A defaulted virtual destructor needed bc of inheritance
    */
   virtual ~Card() = default;

   /*
    *  Basic constructor
    */
   Card(
      PLAYER owner,
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
      std::map< events::EventType, EffectContainer > effects,
      long int mana_cost_delta = 0);

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
         m_mana_cost_ref(card.get_mana_cost_ref()),
         m_mana_cost_base(card.get_mana_cost_base()),
         m_mana_cost_delta(card.get_mana_cost_delta()),
         m_keywords(card.get_keywords()),
         m_effects(card.get_effects_map()),
         m_owner(card.get_owner())
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
};

Card::Card(
   PLAYER owner,
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
   std::map< events::EventType, EffectContainer > effects,
   long int mana_cost_delta)
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
      m_mana_cost_ref(mana_cost),
      m_mana_cost_base(mana_cost),
      m_mana_cost_delta(mana_cost_delta),
      m_keywords(create_kword_list(keyword_list)),
      m_effects(std::move(effects)),
      m_owner(owner)
{
}

class Unit: public Card {
  private:
   // unit cards based attributes
   bool m_alive = true;

   // the fixed reference damage the unit deals.
   const size_t m_power_ref;
   // the permanent power of the unit (can be moved by e.g. effects)
   size_t m_power_base;
   // the fixed reference health of the unit
   const size_t m_health_ref;
   // the permanent base value of the unit (can be moved by e.g. effects)
   size_t m_health_base;

   // The current change in health and damage attributes (hence 'delta')

   // the current change to the unit power (temporary buffs/nerfs).
   long int m_power_delta = 0;
   // the current change to the unit health (temporary buffs/nerfs).
   long int m_health_delta = 0;
   // the damage the unit has taken
   size_t m_damage = 0;

   std::function< void(Game&) > m_last_breath;

  public:
   inline void set_power(size_t power, bool as_delta = true)
   {
      if(as_delta) {
         m_power_delta = static_cast< decltype(m_power_delta) >(
            power - m_power_base);
      } else {
         m_power_base = power;
         m_power_delta = 0;
      }
   }
   inline void set_health(size_t health)
   {
      m_health_delta = static_cast< decltype(m_health_delta) >(
         health - m_health_base);
   }
   inline void die(Game& game)
   {
      if(has_keyword(Keyword::LAST_BREATH)) {
         m_last_breath(game);
      }
      m_alive = false;
   }
   inline void revive() { m_alive = true; }
   inline void set_last_breath(std::function< void(Game&) > lb)
   {
      m_last_breath = std::move(lb);
   }
   [[nodiscard]] inline auto is_damaged() const { return m_damage > 0; }
   [[nodiscard]] inline auto get_power_ref() const { return m_power_ref; }
   [[nodiscard]] inline auto get_power_base() const { return m_power_base; }
   [[nodiscard]] inline auto get_power_delta() const { return m_power_delta; }
   [[nodiscard]] inline auto get_power() const
   {
      return std::max(size_t(0), m_power_base + m_power_delta);
   }
   [[nodiscard]] inline auto get_health() const
   {
      return m_health_base + m_health_delta - m_damage;
   }
   [[nodiscard]] inline auto get_health_ref() const { return m_health_ref; }
   [[nodiscard]] inline auto get_health_base() const { return m_health_base; }
   [[nodiscard]] inline auto get_health_delta() const { return m_health_delta; }
   [[nodiscard]] inline auto get_damage() const { return m_damage; }
   [[nodiscard]] inline auto get_last_breath() const { return m_last_breath; }
   [[nodiscard]] inline bool is_alive() const { return m_alive; }

   inline void take_damage(size_t amount) { m_damage += amount; }
   inline void heal(size_t amount)
   {
      m_damage = std::max(size_t(0), m_damage - amount);
   }
   inline void regenerate()
   {
      m_damage = 0;
   }
   inline void change_health(long int amount, bool permanent)
   {
      if(permanent) {
         m_health_base += amount;

      } else {
         m_health_delta += amount;
      }
   }
   inline void change_power(long int amount, bool permanent)
   {
      if(permanent) {
         m_power_base += amount;
      } else {
         m_power_delta += amount;
      }
   }

   Unit(
      PLAYER owner,
      SID id,
      char* const name,
      char* const effect_desc,
      char* const lore,
      Region region,
      Group group,
      UnitType unit_type,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost_ref,
      size_t power_ref,
      size_t health_ref,
      const std::initializer_list< Keyword >& keyword_list,
      std::map< events::EventType, EffectContainer > effects,
      std::function< void(Game&) > last_breath = [](Game& g) { return; })
       : Card(
          owner,
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
          mana_cost_ref,
          keyword_list,
          std::move(effects)),
         m_power_ref(power_ref),
         m_power_base(power_ref),
         m_health_ref(health_ref),
         m_health_base(health_ref),
         m_last_breath(std::move(last_breath))
   {
   }

   Unit(const Unit& card)
       : Card(card),
         m_power_ref(card.get_power_ref()),
         m_power_base(card.get_power_base()),
         m_health_ref(card.get_health_ref()),
         m_health_base(card.get_health_base()),
         m_power_delta(card.get_power_delta()),
         m_health_delta(card.get_health_delta()),
         m_damage(card.get_damage()),
         m_last_breath(card.get_last_breath())
   {
   }
   Unit& operator=(const Unit& unit) = delete;
   ~Unit() override = default;
   Unit(Unit&&) = delete;
   Unit& operator=(Unit&&) = delete;
};

class Spell: public Card {
   Spell(
      PLAYER owner,
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
      std::map< events::EventType, EffectContainer > effects,
      int mana_cost_delta = 0)
       : Card(
          owner,
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

sptr<Unit> to_unit(const sptr<Card> & card) {
   return std::dynamic_pointer_cast<Unit>(card);
}
sptr<Spell> to_spell(const sptr<Card> & card) {
   return std::dynamic_pointer_cast<Spell>(card);
}

// using VariantCard = std::variant <Follower, Champion, Spell>;

#endif  // LORAINE_CARD_H
