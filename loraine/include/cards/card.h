
#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "effect.h"
#include "event/event.h"
#include "event/event_types.h"
#include "keywords.h"
#include "region.h"
#include "types.h"
#include "uuid_gen.h"

// forward-declarations
class State;
class Grant;
class Game;

/*
 * Abstract base class (abc) for LOR cards.
 */
class Card {
  public:
   enum struct Rarity { NONE, COMMON, RARE, EPIC, CHAMPION };
   enum struct Group {
      NONE,
      DRAGON,
      ELITE,
      ELNUK,
      PORO,
      SEA_MONSTER,
      SPIDER,
      TECH,
      TREASURE,
      YETI,
   };
   enum struct Type { SPELL, UNIT, LANDMARK };
   enum struct SuperType { NONE, SKILL, CHAMPION };

   inline bool operator()(Game& game, const events::AnyEvent& event)
   {
      bool all_consumed = true;
      for(auto& effect : m_effects.at(event.get_event_type())) {
         if(not effect.is_consumed()) {
            effect(game, event);
            all_consumed = false;
         }
      }
      return all_consumed;
   }

   [[nodiscard]] inline auto get_name() const { return m_name; }
   [[nodiscard]] inline auto get_effect_desc() const { return m_effect_desc; }
   [[nodiscard]] inline auto get_lore() const { return m_lore; }
   [[nodiscard]] inline auto get_region() const { return m_region; }
   [[nodiscard]] inline auto get_group() const { return m_group; }
   [[nodiscard]] inline auto get_super_type() const { return m_super_type; }
   [[nodiscard]] inline auto get_rarity() const { return m_rarity; }
   [[nodiscard]] inline auto get_card_type() const { return m_card_type; }
   [[nodiscard]] inline auto get_mana_cost_ref() const { return m_mana_cost_ref; }
   [[nodiscard]] inline auto get_mana_cost_base() const { return m_mana_cost_base; }
   [[nodiscard]] inline auto get_mana_cost_delta() const { return m_mana_cost_delta; }
   [[nodiscard]] inline auto get_mana_cost() const
   {
      return std::max(0L, m_mana_cost_base + m_mana_cost_delta);
   }
   [[nodiscard]] inline const auto& get_keywords() const { return m_keywords; }
   [[nodiscard]] inline auto& get_keywords() { return m_keywords; }
   [[nodiscard]] inline auto& get_effects_map() { return m_effects; }
   [[nodiscard]] inline const auto& get_effects_map() const { return m_effects; }
   [[nodiscard]] inline auto get_id() const { return m_code; }
   [[nodiscard]] inline auto get_uuid() const { return m_uuid; }
   [[nodiscard]] inline Player get_owner() const { return m_owner; }
   [[nodiscard]] inline auto& get_grants() const { return m_grants; }
   [[nodiscard]] inline auto& get_grants_temp() const { return m_grants_temp; }

   // status requests

   [[nodiscard]] inline auto is_hidden() const { return m_hidden; }
   [[nodiscard]] inline auto is_collectible() const { return m_is_collectible; }
   [[nodiscard]] virtual bool is_unit() const { return false; }
   [[nodiscard]] virtual bool is_spell() const { return false; }
   [[nodiscard]] virtual bool is_skill() const { return false; }
   [[nodiscard]] virtual bool is_champion() const { return false; }
   [[nodiscard]] virtual bool is_landmark() const { return false; }
   [[nodiscard]] virtual bool is_follower() const { return false; }

   inline void reduce_mana_cost(long int amount) { m_mana_cost_delta += amount; }
   inline void add_mana_cost(long int amount, bool permanent)
   {
      if(permanent) {
         m_mana_cost_base += amount;
      } else {
         m_mana_cost_delta += amount;
      }
   }
   inline void set_flag_hidden(bool value) { m_hidden = value; }
   inline void set_keywords(KeywordMap keywords) { m_keywords = keywords; }

   // manipulations

   void set_effect_vec(events::EventType e_type, std::vector< EffectContainer > effects);

   void add_effect(events::EventType e_type, EffectContainer effect);

   void remove_effect(events::EventType e_type, const EffectContainer& effect);

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

   inline bool operator==(const Card& rhs) const { return m_uuid == rhs.get_uuid(); }
   inline bool operator!=(const Card& rhs) const { return *this == rhs; }

   /*
    * This function returns the boolean indicator for whether the current card has a play condition
    * that can be fulfilled at the present moment.
    */
   [[nodiscard]] bool check_play_condition(const Game& game) const;

   /*
    * The play condition function represents a potential 'cost' the player has to pay to play this
    * card (e.g. discarding another card in hand)
    */
   virtual void play_condition(Game& /*unused*/) {}

   /*
    * A defaulted virtual destructor needed bc of inheritance
    */
   virtual ~Card() = default;

   /*
    *  Basic constructor
    */
   Card(
      Player owner,
      const char* const code,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      SuperType super_type,
      Rarity rarity,
      Type card_type,
      bool is_collectible,
      size_t mana_cost,
      std::initializer_list< Keyword > keyword_list,
      std::map< events::EventType, std::vector< EffectContainer > > effects,
      bool is_hidden = true);

   /*
    * Copy Constructor
    */
   Card(const Card& card);

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
   // the super type (champion, skill, none)
   const SuperType m_super_type;
   // the rarity of the cards
   const Rarity m_rarity;
   // whether the cards is a spell or a unit
   const Type m_card_type;
   // whether a card is collectible (i.e. can be added to a deck)
   const bool m_is_collectible;

   // the card code
   const char* const m_code;
   // the unique id used to identify this specific instance of a card
   const UUID m_uuid;

   // variable attributes of the cards

   bool m_hidden;
   // the mana it costs to play the cards (as default)
   const size_t m_mana_cost_ref;
   // when effects move the base cost to a new permanent value
   long int m_mana_cost_base;
   // the current change to the mana cost of the card
   long int m_mana_cost_delta;
   // all the keywords pertaining to the cards
   KeywordMap m_keywords;

   // all possible effects
   std::map< events::EventType, std::vector< EffectContainer > > m_effects;

   // all permanent grants
   std::vector< sptr< Grant > > m_grants;
   // all temporary grants
   std::vector< sptr< Grant > > m_grants_temp;

   // the player whose card this is
   Player m_owner;

   [[nodiscard]] virtual bool _check_play_condition(const Game& game) const = 0;
};

class Unit: public Card {
  private:
   // unit cards based attributes
   bool m_alive = true;
   bool m_stunned = false;

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

   [[nodiscard]] bool _check_play_condition(const Game& game) const override;

  public:
   inline void set_stunned(bool value) { m_stunned = value; }
   inline void die() { m_alive = false; }
   inline void revive() { m_alive = true; }
   void set_power(size_t power, bool as_delta = true);
   void set_health(size_t health);
   void add_power(long int amount, bool permanent);
   void add_health(long int amount, bool permanent);
   inline void take_damage(size_t amount) { m_damage += amount; }
   inline void heal(size_t amount) { m_damage -= std::min(m_damage, amount); }

   // status requests

   [[nodiscard]] bool is_unit() const override { return true; }

   [[nodiscard]] inline auto is_damaged() const { return m_damage > 0; }

   [[nodiscard]] inline auto get_power_raw() const
   {
      return m_power_base + static_cast< size_t >(m_power_delta);
   }
   [[nodiscard]] inline auto get_power() const { return std::max(size_t(0), get_power_raw()); }
   [[nodiscard]] inline auto get_power_ref() const { return m_power_ref; }
   [[nodiscard]] inline auto get_power_base() const { return m_power_base; }
   [[nodiscard]] inline auto get_power_delta() const { return m_power_delta; }

   [[nodiscard]] inline auto get_health_raw() const
   {
      long health_base = static_cast< long int >(m_health_base);
      long damage = static_cast< long >(m_damage);
      return health_base + m_health_delta - damage;
   }
   [[nodiscard]] inline auto get_health() const { return std::max(long(0), get_health_raw()); }
   [[nodiscard]] inline auto get_health_ref() const { return m_health_ref; }
   [[nodiscard]] inline auto get_health_base() const { return m_health_base; }
   [[nodiscard]] inline auto get_health_delta() const { return m_health_delta; }

   [[nodiscard]] inline auto get_damage() const { return m_damage; }
   [[nodiscard]] inline bool is_alive() const { return m_alive; }

   Unit(
      Player owner,
      const char* const code,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      SuperType super_type,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost_ref,
      size_t power_ref,
      size_t health_ref,
      const std::initializer_list< Keyword >& keyword_list,
      const std::map< events::EventType, std::vector< EffectContainer > >& effects,
      Type card_type = Card::Type::UNIT);

   Unit(const Unit& card);
   Unit& operator=(const Unit& unit) = delete;
   ~Unit() override = default;
   Unit(Unit&&) = delete;
   Unit& operator=(Unit&&) = delete;
};

class Spell: public Card {
   [[nodiscard]] bool _check_play_condition(const Game& game) const override;

  public:
   Spell(
      Player owner,
      const char* const code,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      SuperType super_type,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost,
      const std::initializer_list< Keyword >& keyword_list,
      const std::map< events::EventType, std::vector< EffectContainer > >& effects);

   [[nodiscard]] bool is_spell() const override { return true; }
};

class Landmark: public Unit {
  public:
   Landmark(
      Player owner,
      const char* const code,
      const char* const name,
      const char* const effect_desc,
      const char* const lore,
      Region region,
      Group group,
      Rarity rarity,
      bool is_collectible,
      size_t mana_cost,
      std::initializer_list< Keyword > keyword_list,
      const std::map< events::EventType, std::vector< EffectContainer > >& effects);

   [[nodiscard]] bool is_landmark() const override { return true; }
   [[nodiscard]] bool is_unit() const override { return false; }
};

inline sptr< Unit > to_unit(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Unit >(card);
}
inline sptr< Spell > to_spell(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Spell >(card);
}
inline sptr< Landmark > to_landmark(const sptr< Card >& card)
{
   return std::dynamic_pointer_cast< Landmark >(card);
}

// hash specializations
namespace std {
template <>
struct hash< Card > {
   size_t operator()(const Card& x) const { return std::hash< UUID >()(x.get_uuid()); }
};
template <>
struct hash< sptr< Card > > {
   size_t operator()(const sptr< Card >& x) const { return std::hash< Card >()(*x); }
};
}  // namespace std

// using VariantCard = std::variant <Follower, Champion, Spell>;

#endif  // LORAINE_CARD_H
