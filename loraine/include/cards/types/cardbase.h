
#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "cards/card_defs.h"
#include "cards/effect.h"
#include "events/event.h"
#include "events/event_listener.h"
#include "events/event_types.h"
#include "types.h"
#include "uuid_gen.h"

// forward-declarations
class State;
class Grant;

/*
 * Abstract base class (abc) for LOR cards.
 */
class Card : public EventListener<Card> {
  public:

   struct ConstData {
      // the card code
      const char* const code;
      // the cards' name
      const char* const name;
      // the description text of the cards
      const char* const effect_desc;
      // text of the cards in context of the lol universe
      const char* const lore;
      // the region the cards is from
      const Region region;
      // the subgroup the cards belongs to
      const Group group;
      // the super type (champion, skill, none)
      const CardSuperType super_type;
      // the rarity of the cards
      const Rarity rarity;
      // whether the cards is a spell or a unit
      const CardType card_type;
      // the mana it costs to play the cards (as default)
      const size_t mana_cost_ref;
      // whether a card is collectible (i.e. can be added to a deck)
      const bool is_collectible = true;
      // was the card created by another
      const std::optional< const char* const> creator = {};
      // the unique id used to identify this specific instance of a card
      const UUID uuid = new_uuid();
   };

   struct MutableData {
      // the player whose card this is
      Player owner;
      // the current location of the card in the game
      Location location;
      // the index in the current location
      size_t position;
      // whether the card is observable by all or only by the owner
      bool hidden;
      // when effects move the base cost to a new permanent value
      long int mana_cost_base = 0;
      // the current change to the mana cost of the card
      long int mana_cost_delta = 0;
      // all the keywords pertaining to the cards
      KeywordMap keywords = {};
      // all effects
      std::map< events::EventType, std::vector< Effect > > effects = {};
      // all permanent grants
      std::vector< sptr< Grant > > grants = {};
      // all temporary grants
      std::vector< sptr< Grant > > grants_temp = {};
   };

   [[nodiscard]] inline auto& immutables() const { return m_immutables; }
   [[nodiscard]] inline auto& immutables() { return m_immutables; }
   [[nodiscard]] inline auto& mutables() const { return m_mutables; }
   [[nodiscard]] inline auto& mutables() { return m_mutables; }
   [[nodiscard]] inline auto mana_cost() const
   {
      return std::max(0L, m_mutables.mana_cost_base + m_mutables.mana_cost_delta);
   }
   void effects(events::EventType e_type, std::vector< Effect > effects);
   [[nodiscard]] inline auto& effects(events::EventType etype)
   {
      return m_mutables.effects.at(etype);
   }
   [[nodiscard]] inline auto& effects(events::EventType etype) const
   {
      return m_mutables.effects.at(etype);
   }

   [[nodiscard]] std::vector< sptr< Grant > > all_grants() const;

   // status requests

   [[nodiscard]] virtual bool is_unit() const { return false; }
   [[nodiscard]] virtual bool is_spell() const { return false; }
   [[nodiscard]] virtual bool is_skill() const { return false; }
   [[nodiscard]] virtual bool is_fieldcard() const { return false; }
   [[nodiscard]] virtual bool is_champion() const { return false; }
   [[nodiscard]] virtual bool is_landmark() const { return false; }
   [[nodiscard]] virtual bool is_follower() const { return false; }

   [[nodiscard]] bool is_created() const { return has_value(m_immutables.creator); }
   [[nodiscard]] auto creator() const { return m_immutables.creator.value(); }

   inline void reduce_mana_cost(long int amount) { m_mutables.mana_cost_delta -= amount; }
   inline void move(Location loc, size_t index)
   {
      m_mutables.location = loc;
      m_mutables.position = index;
   }

   [[nodiscard]] inline bool has_keyword(Keyword kword) const
   {
      return m_mutables.keywords.at(static_cast< unsigned long >(kword));
   }
   [[nodiscard]] inline bool has_effect(events::EventType e_type) const
   {
      return m_mutables.effects.find(e_type) != m_mutables.effects.end();
   }
   [[nodiscard]] inline bool has_effect(events::EventType e_type, const Effect& effect) const;

   // manipulations

   void add_effect(events::EventType e_type, Effect effect);
   void remove_effect(events::EventType e_type, const Effect& effect);

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
    * This function returns the boolean indicator for whether the current card has a play condition
    * that can be fulfilled at the present moment.
    */
   [[nodiscard]] bool check_play_tribute(const State& state) const;

   /*
    * The play condition function represents a potential 'cost' the player has to pay to play this
    * card (e.g. discarding another card in hand)
    */
   virtual inline void pay_play_tribute(State& /*unused*/) {}

   /*
    * A defaulted virtual destructor needed bc of inheritance
    */
   virtual ~Card() = default;

   /*
    *  Basic constructor
    */
   Card(ConstData const_attrs, MutableData var_attrs);

   /*
    * Copy Constructor
    */
   Card(const Card& card) = default;

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
   // fixed attributes of the card
   ConstData m_immutables;
   // variable attributes of the card
   MutableData m_mutables;

   [[nodiscard]] virtual bool _check_play_condition(const State& state) const = 0;
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
