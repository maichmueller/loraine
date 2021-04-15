
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
class CardAttributes {
  public:
   CardAttributes(
      std::string name,
      std::string code,
      std::string description,
      std::string lore,
      Region region,
      Group group,
      CardSuperType super_type,
      Rarity rarity,
      CardType card_type,
      bool is_collectible,
      bool hidden = true,
      std::optional< entt::entity > creator = std::nullopt)
       : m_name(name),
         m_code(code),
         m_description(description),
         m_lore(lore),
         m_region(region),
         m_group(group),
         m_super_type(super_type),
         m_rarity(rarity),
         m_card_type(card_type),
         m_is_collectible(is_collectible),
         m_creator(creator),
         m_hidden(hidden)
   {
   }

  private:
   // the cards' name
   const std::string m_name;
   // the card's code in the LoR format, e.g. "DE01012"
   const std::string m_code;
   // the description text of the cards for their effects
   const std::string m_description;
   // text of the cards in context of the lol universe
   const std::string m_lore;
   // the region the cards is from
   const Region m_region;
   // the subgroup the cards belongs to
   const Group m_group;
   // the super type (champion, skill, none)
   const CardSuperType m_super_type;
   // the rarity of the cards
   const Rarity m_rarity;
   // whether the card is a spell, unit, landmark, or trap
   const CardType m_card_type;
   // whether a spell is collectible (i.e. can be added to a deck)
   const bool m_is_collectible;
   // was the card created by another
   const std::optional< entt::entity > m_creator;
   // whether the card is observable by all or only by its owner
   bool m_hidden;
};

class ManaCost {
  public:
   ManaCost(size_t cost_ref, long delta = 0)
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

class Position {
  public:
   Position(Zone zone, size_t index) : m_location(zone), m_index(index) {}

   inline Zone zone() const { return m_location; }
   inline size_t index() const { return m_index; }

  private:
   // the current location of the spell in the game
   Zone m_location;
   // the index in the current location
   size_t m_index;
};

class KeywordMap {
  public:
   KeywordMap() : m_kw_arr() {}  // all keyword positions marked false by default
   constexpr KeywordMap(std::array< bool, n_keywords > keywords) : m_kw_arr(keywords) {}
   template <typename Container>
   KeywordMap(Container keywords) : m_kw_arr()
   {
      algo::for_each([&](Keyword kw) { add_keyword(kw); }, keywords);
   }

  private:
   inline size_t to_int(Keyword kw) const { return static_cast< size_t >(kw); }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const { return m_kw_arr.at(to_int(kword)); }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   [[nodiscard]] inline bool has_any_keyword(Container kwords) const
   {
      return algo::any_of(kwords, [&](const auto& kw) { return has_keyword(kw); });
   }

   inline void add_keyword(Keyword kw) { m_kw_arr[to_int(kw)] = true; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void add_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::add_keyword, kwords);
   }
   inline void remove_keyword(Keyword kw) { m_kw_arr[to_int(kw)] = false; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void remove_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::remove_keyword, kwords);
   }

  private:
   std::array< bool, n_keywords > m_kw_arr;
};

class EffectMap {
  public:
   EffectMap() : m_kw_arr() {}  // all keyword positions marked false by default
   constexpr EffectMap(std::array< bool, n_keywords > keywords) : m_kw_arr(keywords) {}
   template <typename Container>
   EffectMap(Container keywords) : m_kw_arr()
   {
      algo::for_each([&](Keyword kw) { add_keyword(kw); }, keywords);
   }

  private:
   inline size_t to_int(Keyword kw) const { return static_cast< size_t >(kw); }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const { return m_kw_arr.at(to_int(kword)); }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   [[nodiscard]] inline bool has_any_keyword(Container kwords) const
   {
      return algo::any_of(kwords, [&](const auto& kw) { return has_keyword(kw); });
   }

   inline void add_keyword(Keyword kw) { m_kw_arr[to_int(kw)] = true; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void add_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::add_keyword, kwords);
   }
   inline void remove_keyword(Keyword kw) { m_kw_arr[to_int(kw)] = false; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void remove_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::remove_keyword, kwords);
   }

  private:
   std::array< bool, n_keywords > m_kw_arr;
}

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
   return m_mutables.add_effects.at(etype);
}
[[nodiscard]] inline auto& effects(events::EventLabel etype) const
{
   return m_mutables.add_effects.at(etype);
}
[[nodiscard]] inline auto& effects()
{
   return m_mutables.add_effects;
}
[[nodiscard]] inline auto& effects() const
{
   return m_mutables.add_effects;
}

[[nodiscard]] auto creator() const
{
   return m_immutables.creator_code.value();
}

[[nodiscard]] std::vector< sptr< Grant > > all_grants() const;

// status requests
[[nodiscard]] inline bool is_spell() const
{
   return m_immutables.card_type == CardType::SPELL;
}
[[nodiscard]] inline bool is_unit() const
{
   return m_immutables.card_type == CardType::UNIT;
}
[[nodiscard]] inline bool is_landmark() const
{
   return m_immutables.card_type == CardType::LANDMARK;
}
[[nodiscard]] inline bool is_fieldcard() const
{
   return is_unit() || is_landmark();
}
[[nodiscard]] inline bool is_trap() const
{
   return m_immutables.card_type == CardType::TRAP;
}
[[nodiscard]] inline bool is_skill() const
{
   return m_immutables.super_type == CardSuperType::SKILL;
}
[[nodiscard]] inline bool is_champion() const
{
   return m_immutables.super_type == CardSuperType::CHAMPION;
}
[[nodiscard]] inline bool is_follower() const
{
   return is_unit() && not is_champion();
}
[[nodiscard]] bool is_created() const
{
   return utils::has_value(m_immutables.creator_code);
}

[[nodiscard]] inline bool has_effect(events::EventLabel e_type) const
{
   return m_mutables.add_effects.find(e_type) != m_mutables.add_effects.end();
}

[[nodiscard]] bool has_effect(events::EventLabel e_type, const IEffect& effect) const;

// manipulations

inline void uncover()
{
   m_mutables.hidden = false;
}

void add_effect(events::EventLabel e_type, sptr< IEffect > effect);
void remove_effect(events::EventLabel e_type, const IEffect& effect);

void store_grant(const sptr< Grant >& grant);
inline void store_grant(const std::vector< sptr< Grant > >& grants)
{
   for(const auto& grant : grants) {
      store_grant(grant);
   }
}

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
}
;

#endif  // LORAINE_BASE_H
