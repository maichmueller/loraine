
#ifndef LORAINE_COMPONENTS_H
#define LORAINE_COMPONENTS_H

#include "carddefs.h"
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
       : name(name),
         code(code),
         description(description),
         lore(lore),
         region(region),
         group(group),
         super_type(super_type),
         rarity(rarity),
         card_type(card_type),
         is_collectible(is_collectible),
         creator(creator),
         hidden(hidden)
   {
   }

   /// the cards' name
   const std::string name;
   /// the card's code in the LoR format, e.g. "DE01012"
   const std::string code;
   /// the description text of the cards for their effects
   const std::string description;
   /// text of the cards in context of the lol universe
   const std::string lore;
   /// the region the cards is from
   const Region region;
   /// the subgroup the cards belongs to
   const Group group;
   /// the super type (champion, skill, none)
   const CardSuperType super_type;
   /// the rarity of the cards
   const Rarity rarity;
   /// whether the card is a spell, unit, landmark, or trap
   const CardType card_type;
   /// whether a spell is collectible (i.e. can be added to a deck)
   const bool is_collectible;
   /// was the card created by another
   const std::optional< entt::entity > creator;
   /// whether the card is observable by all or only by its owner
   bool hidden;
};

class ManaCost {
  public:
   ManaCost(long cost_ref) : m_cost_ref(cost_ref), m_cost_cache(cost_ref) {}

   inline long cost() const { return m_cost_cache; }
   inline void modify(long amount)
   {
      m_cost_delta += amount;
      m_cost_cache = std::max(0L, m_cost_ref + m_cost_delta);
   }

  private:
   /// the mana it costs to play the card (as default)
   const long m_cost_ref;
   /// the actual cost at the moment
   long m_cost_cache;
   /// the change to the mana cost of the card
   long m_cost_delta = 0;
};

class PowerStat {
  public:
   PowerStat(long power_ref) : m_power_ref(power_ref), m_power_cache(power_ref) {}

   inline long power() const { return m_power_cache; }
   inline void modify(long amount)
   {
      m_power_delta += amount;
      m_power_cache = std::max(0L, m_power_ref + m_power_delta);
   }

  private:
   /// the mana it costs to play the card (as default)
   const long m_power_ref;
   /// the actual cost at the moment
   long m_power_cache;
   /// the change to the mana cost of the card
   long int m_power_delta = 0;
};

class HealthStat {
  public:
   HealthStat(long power_ref) : m_health_ref(power_ref), m_health_cache(power_ref) {}

   inline long power() const { return m_health_cache; }
   inline void modify(long amount, bool temporary)
   {
      if(temporary) {
         m_power_delta_temp += amount;
      } else {
         m_power_delta += amount;
      }
      m_health_cache = std::max(0L, m_health_ref + m_power_delta + m_power_delta + m_damage);
   }
   inline void damage(unsigned int amount) { m_damage += amount; }
   inline void heal(unsigned int amount) { m_damage -= std::min(amount, m_damage); }

  private:
   /// the mana it costs to play the card (as default)
   const long m_health_ref;
   /// the actual cost at the moment
   unsigned int m_health_cache;
   /// the change to the mana cost of the card
   long m_power_delta = 0;
   /// the temporary change to the mana cost of the card, which can reduce damage at round end
   long m_power_delta_temp = 0;
   /// the current damage taken
   unsigned int m_damage = 0;
};

class Position {
  public:
   Position(Zone zone, size_t index) : m_location(zone), m_index(index) {}

   inline Zone zone() const { return m_location; }
   inline size_t index() const { return m_index; }

  private:
   /// the current location of the spell in the game
   Zone m_location;
   /// the index in the current location
   size_t m_index;
};

class KeywordMap {
  public:
   KeywordMap() : m_kw_arr() {}  // all keyword positions marked false by default
   constexpr KeywordMap(std::array< bool, n_keywords > keywords) : m_kw_arr(keywords) {}
   template < typename Container >
   KeywordMap(Container keywords) : m_kw_arr()
   {
      algo::for_each([&](Keyword kw) { add_keyword(kw); }, keywords);
   }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const { return m_kw_arr.at(_to_int(kword)); }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   [[nodiscard]] inline bool has_any_keyword(Container kwords) const
   {
      return algo::any_of(kwords, [&](const auto& kw) { return has_keyword(kw); });
   }

   inline void add_keyword(Keyword kw) { m_kw_arr[_to_int(kw)] = true; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void add_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::add_keyword, kwords);
   }
   inline void remove_keyword(Keyword kw) { m_kw_arr[_to_int(kw)] = false; }
   template < typename Container, typename = std::enable_if_t< Container::value_type, Keyword > >
   inline void remove_keywords(Container kwords)
   {
      algo::transform(&KeywordMap::remove_keyword, kwords);
   }

  private:

   /// members

   std::array< bool, n_keywords > m_kw_arr;

   /// private methods

   inline size_t _to_int(Keyword kw) const { return static_cast< size_t >(kw); }
};

class PlayCondition {
   bool check(GameState& state);

  private:
   entt::entity m_effect;
   entt::entity m_condition;
};

#endif  // LORAINE_COMPONENTS_H
