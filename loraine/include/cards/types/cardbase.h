
#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <cards/toll.h>

#include <entt/entt.hpp>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "cards/card_defs.h"
#include "core/targeting.h"
#include "effects/effect.h"
#include "events/event_labels.h"
#include "events/event_listener.h"
#include "events/event_subscriber.h"
#include "utils/algorithms.h"
#include "utils/types.h"
#include "utils/utils.h"

// forward-declarations

class GameState;
class Grant;

/**
 * Abstract base class (abc) for LOR cards.
 */

struct CardAttributes {
   // the cards' name
   const std::string name;
   // the card's code in the LoR format, e.g. "DE01012"
   const std::string code;
   // the description text of the cards for their effects
   const std::string description;
   // text of the cards in context of the lol universe
   const std::string lore;
   // the region the cards is from
   const Region region;
   // the subgroup the cards belongs to
   const Group group;
   // the super type (champion, skill, none)
   const CardSuperType super_type;
   // the rarity of the cards
   const Rarity rarity;
   // whether the card is a spell, unit, landmark, or trap
   const CardType card_type;
   // whether a spell is collectible (i.e. can be added to a deck)
   const bool is_collectible;
   // was the card created by another
   const std::optional< entt::entity > creator = std::nullopt;

   // whether the card is observable by all or only by its owner
   bool hidden;
};

struct Mana {
   Mana(size_t cost_ref, long delta = 0)
       : m_cost_ref(cost_ref), m_cost_cache(cost_ref), m_cost_delta(0)
   {
      if(delta != 0) {
         add(delta);
      }
   }

   inline size_t cost() const { return m_cost_cache; }
   inline void add(long amount)
   {
      m_cost_delta += amount;
      m_cost_cache = std::max(0L, (long) m_cost_ref + m_cost_delta);
   }

  private:
   // the mana it costs to play the card (as default)
   const size_t m_cost_ref;
   // the actual cost at the moment
   size_t m_cost_cache;
   // the change to the mana cost of the card
   long int m_cost_delta = 0;
};

struct Position {
   Position(Zone zone, size_t index) : m_location(zone), m_index(index) {}

   inline Zone
  private:
   // the current location of the spell in the game
   Zone m_location;
   // the index in the current location
   size_t m_index;
};

class Card: public Cloneable< abstract_method< Card > >, public Targetable {
  public:
   using EffectMap = std::map< events::EventLabel, std::vector< sptr< IEffect > > >;

   struct MutableState {
      // all the m_keywords pertaining to the cards
      KeywordMap keywords = {};
      // all m_effects
      EffectMap effects = {};
      // condition
      uptr< Toll > play_toll = std::make_unique< Toll >();
      // all permanent grants
      std::vector< sptr< Grant > > grants = {};
      // all temporary grants
      std::vector< sptr< Grant > > grants_temp = {};
   };

   [[nodiscard]] inline auto mana_cost() const {}
   void effects(events::EventLabel e_type, std::vector< sptr< IEffect > > effects);
   [[nodiscard]] inline auto& effects(events::EventLabel etype)
   {
      return m_mutables.effects.at(etype);
   }
   [[nodiscard]] inline auto& effects(events::EventLabel etype) const
   {
      return m_mutables.effects.at(etype);
   }
   [[nodiscard]] inline auto& effects() { return m_mutables.effects; }
   [[nodiscard]] inline auto& effects() const { return m_mutables.effects; }

   [[nodiscard]] auto creator() const { return m_immutables.creator_code.value(); }

   [[nodiscard]] std::vector< sptr< Grant > > all_grants() const;

   // status requests
   [[nodiscard]] inline bool is_spell() const { return m_immutables.card_type == CardType::SPELL; }
   [[nodiscard]] inline bool is_unit() const { return m_immutables.card_type == CardType::UNIT; }
   [[nodiscard]] inline bool is_landmark() const
   {
      return m_immutables.card_type == CardType::LANDMARK;
   }
   [[nodiscard]] inline bool is_fieldcard() const { return is_unit() || is_landmark(); }
   [[nodiscard]] inline bool is_trap() const { return m_immutables.card_type == CardType::TRAP; }
   [[nodiscard]] inline bool is_skill() const
   {
      return m_immutables.super_type == CardSuperType::SKILL;
   }
   [[nodiscard]] inline bool is_champion() const
   {
      return m_immutables.super_type == CardSuperType::CHAMPION;
   }
   [[nodiscard]] inline bool is_follower() const { return is_unit() && not is_champion(); }
   [[nodiscard]] bool is_created() const { return utils::has_value(m_immutables.creator_code); }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const
   {
      return m_mutables.keywords.at(static_cast< unsigned long >(kword));
   }
   [[nodiscard]] inline bool has_any_keyword(std::initializer_list< Keyword > kwords) const
   {
      return algo::any_of(kwords, [&](const auto& kw) {
         return m_mutables.keywords.at(static_cast< unsigned long >(kw));
      });
   }
   [[nodiscard]] inline bool has_effect(events::EventLabel e_type) const
   {
      return m_mutables.effects.find(e_type) != m_mutables.effects.end();
   }

   [[nodiscard]] bool has_effect(events::EventLabel e_type, const IEffect& effect) const;
   inline void reduce_mana_cost(long int amount) { m_mutables.mana_cost_delta -= amount; }

   inline void move(Zone loc, size_t index)
   {
      m_mutables.location = loc;
      m_mutables.position = index;
   }

   // manipulations

   inline void uncover() { m_mutables.hidden = false; }

   void add_effect(events::EventLabel e_type, sptr< IEffect > effect);
   void remove_effect(events::EventLabel e_type, const IEffect& effect);

   void store_grant(const sptr< Grant >& grant);
   inline void store_grant(const std::vector< sptr< Grant > >& grants)
   {
      for(const auto& grant : grants) {
         store_grant(grant);
      }
   }
   inline void add_keyword(Keyword kword)
   {
      m_mutables.keywords[static_cast< unsigned long >(kword)] = true;
   }
   inline void remove_keyword(Keyword kword)
   {
      m_mutables.keywords[static_cast< unsigned long >(kword)] = false;
   }
   inline void add_mana_cost(long int amount, bool permanent)
   {
      if(permanent) {
         m_mutables.mana_cost_base += amount;
      } else {
         m_mutables.mana_cost_delta += amount;
      }
   }

   inline bool operator==(const Card& rhs) const
   {
      return m_immutables.uuid == rhs.immutables().uuid;
   }
   inline bool operator!=(const Card& rhs) const { return not (*this == rhs); }

   /*
    * A defaulted virtual destructor needed bc of inheritance
    */
   ~Card() override = default;

   /*
    *  Basic constructor
    */
   Card(ConstState const_attrs, MutableState var_attrs);

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
   // fixed attributes of the spell
   const ConstState m_immutables;
   // variable attributes of the spell
   MutableState m_mutables;

   EffectMap _clone_effect_map(const EffectMap& emap);
};

template < typename T >
inline sptr< Card > to_card(const sptr< T >& card)
{
   return std::dynamic_pointer_cast< Card >(card);
}

// hash specializations
namespace std {
template <>
struct hash< Card > {
   size_t operator()(const Card& x) const { return std::hash< UUID >()(x.immutables().uuid); }
};
template <>
struct hash< sptr< Card > > {
   size_t operator()(const sptr< Card >& x) const { return std::hash< Card >()(*x); }
};
}  // namespace std

#endif  // LORAINE_BASE_H
