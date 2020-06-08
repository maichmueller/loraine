//
// Created by michael on 29.05.20.
//

#ifndef LORAINE_CARD_H
#define LORAINE_CARD_H

#include <utility>
#include <vector>

#include "event.h"
#include "keywords.h"
#include "region.h"
#include "types.h"
#include "uuid_gen.h"

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

class Wonder {
  public:
   auto get_effect() { return m_effect; }
   void set_effect(
      const std::function< void(State&, const events::VariantEvent&) >& effect)
   {
      m_effect = effect;
   }
   void operator()(State& state, const events::VariantEvent& event)
   {
      m_effect(state, event);
   }

   Wonder() : m_effect([](State& state, const events::VariantEvent& event) {})
   {
   }  // empty wonder
   explicit Wonder(
      std::function< void(State&, const events::VariantEvent&) > effect)
       : m_effect(std::move(effect))
   {
   }

  private:
   std::function< void(State&, const events::VariantEvent&) > m_effect;
};

/*
 * Abstract base class (abc) for LOR cards.
 */
class Card {
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

   const SID m_id;
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
   [[maybe_unused]] Wonder m_on_battle = Wonder();
   [[maybe_unused]] Wonder m_on_cast = Wonder();
   [[maybe_unused]] Wonder m_on_decl_attack = Wonder();
   [[maybe_unused]] Wonder m_on_decl_block = Wonder();
   [[maybe_unused]] Wonder m_on_die = Wonder();
   [[maybe_unused]] Wonder m_on_draw_card = Wonder();
   [[maybe_unused]] Wonder m_on_end_round = Wonder();
   [[maybe_unused]] Wonder m_on_enlightenment = Wonder();
   [[maybe_unused]] Wonder m_on_gain_mana = Wonder();
   [[maybe_unused]] Wonder m_on_game_end = Wonder();
   [[maybe_unused]] Wonder m_on_get_card = Wonder();
   [[maybe_unused]] Wonder m_on_level_up = Wonder();
   [[maybe_unused]] Wonder m_on_nexus_damage = Wonder();
   [[maybe_unused]] Wonder m_on_nexus_strike = Wonder();
   [[maybe_unused]] Wonder m_on_play_unit = Wonder();
   [[maybe_unused]] Wonder m_on_play_spell = Wonder();
   [[maybe_unused]] Wonder m_on_recall = Wonder();
   [[maybe_unused]] Wonder m_on_start_round = Wonder();
   [[maybe_unused]] Wonder m_on_strike = Wonder();
   [[maybe_unused]] Wonder m_on_stun = Wonder();
   [[maybe_unused]] Wonder m_on_summon = Wonder();
   [[maybe_unused]] Wonder m_on_target = Wonder();
   [[maybe_unused]] Wonder m_on_unit_take_damage = Wonder();
   [[maybe_unused]] Wonder m_on_use_mana = Wonder();

  public:
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

   [[nodiscard]] auto get_on_battle() const { return m_on_battle; }
   [[nodiscard]] auto get_on_cast() const { return m_on_cast; }
   [[nodiscard]] auto get_on_decl_attack() const { return m_on_decl_attack; }
   [[nodiscard]] auto get_on_decl_block() const { return m_on_decl_block; }
   [[nodiscard]] auto get_on_die() const { return m_on_die; }
   [[nodiscard]] auto get_on_draw_card() const { return m_on_draw_card; }
   [[nodiscard]] auto get_on_end_round() const { return m_on_end_round; }
   [[nodiscard]] auto get_on_enlightenment() const
   {
      return m_on_enlightenment;
   }
   [[nodiscard]] auto get_on_gain_mana() const { return m_on_gain_mana; }
   [[nodiscard]] auto get_on_game_end() const { return m_on_game_end; }
   [[nodiscard]] auto get_on_get_card() const { return m_on_get_card; }
   [[nodiscard]] auto get_on_level_up() const { return m_on_level_up; }
   [[nodiscard]] auto get_on_nexus_damage() const { return m_on_nexus_damage; }
   [[nodiscard]] auto get_on_nexus_strike() const { return m_on_nexus_strike; }
   [[nodiscard]] auto get_on_play_unit() const { return m_on_play_unit; }
   [[nodiscard]] auto get_on_play_spell() const { return m_on_play_spell; }
   [[nodiscard]] auto get_on_recall() const { return m_on_recall; }
   [[nodiscard]] auto get_on_start_round() const { return m_on_start_round; }
   [[nodiscard]] auto get_on_strike() const { return m_on_strike; }
   [[nodiscard]] auto get_on_stun() const { return m_on_stun; }
   [[nodiscard]] auto get_on_summon() const { return m_on_summon; }
   [[nodiscard]] auto get_on_target() const { return m_on_target; }
   [[nodiscard]] auto get_on_unit_take_damage() const
   {
      return m_on_unit_take_damage;
   }
   [[nodiscard]] auto get_on_use_mana() const { return m_on_use_mana; }

   auto set_on_battle(Wonder on_battle) { m_on_battle = std::move(on_battle); }
   auto set_on_cast(Wonder on_cast) { m_on_cast = std::move(on_cast); }
   auto set_on_attack(Wonder on_decl_attack)
   {
      m_on_decl_attack = std::move(on_decl_attack);
   }
   auto set_on_decl_block(Wonder on_decl_block)
   {
      m_on_decl_block = std::move(on_decl_block);
   }
   auto set_on_die(Wonder on_die) { m_on_die = std::move(on_die); }
   auto set_on_draw_card(Wonder on_draw_card)
   {
      m_on_draw_card = std::move(on_draw_card);
   }
   auto set_on_end_round(Wonder on_end_round)
   {
      m_on_end_round = std::move(on_end_round);
   }
   auto set_on_enlightenment(Wonder on_enlightenment)
   {
      m_on_enlightenment = std::move(on_enlightenment);
   }
   auto set_on_gain_mana(Wonder on_gain_mana)
   {
      m_on_gain_mana = std::move(on_gain_mana);
   }
   auto set_on_game_end(Wonder on_game_end)
   {
      m_on_game_end = std::move(on_game_end);
   }
   auto set_on_get_card(Wonder on_get_card)
   {
      m_on_get_card = std::move(on_get_card);
   }
   auto set_on_level_up(Wonder on_level_up)
   {
      m_on_level_up = std::move(on_level_up);
   }
   auto set_on_nexus_damage(Wonder on_nexus_damage)
   {
      m_on_nexus_damage = std::move(on_nexus_damage);
   }
   auto set_on_nexus_strike(Wonder on_nexus_strike)
   {
      m_on_nexus_strike = std::move(on_nexus_strike);
   }
   auto set_on_play_unit(Wonder on_play_unit)
   {
      m_on_play_unit = std::move(on_play_unit);
   }
   auto set_on_play_spell(Wonder on_play_spell)
   {
      m_on_play_spell = std::move(on_play_spell);
   }
   auto set_on_recall(Wonder on_recall) { m_on_recall = std::move(on_recall); }
   auto set_on_start_round(Wonder on_start_round)
   {
      m_on_start_round = std::move(on_start_round);
   }
   auto set_on_strike(Wonder on_strike) { m_on_strike = std::move(on_strike); }
   auto set_on_stun(Wonder on_stun) { m_on_stun = std::move(on_stun); }
   auto set_on_summon(Wonder on_summon) { m_on_summon = std::move(on_summon); }
   auto set_on_target(Wonder on_target) { m_on_target = std::move(on_target); }
   auto set_on_unit_take_damage(Wonder on_unit_take_damage)
   {
      m_on_unit_take_damage = std::move(on_unit_take_damage);
   }
   auto set_on_use_mana(Wonder on_use_mana)
   {
      m_on_use_mana = std::move(on_use_mana);
   }

   void set_mana_cost(u64 mana_cost) { m_mana_cost = mana_cost; }
   void set_damage(u64 damage) { m_damage = damage; }
   void set_health(u64 health) { m_health = health; }
   void set_keywords(std::vector< Keyword > kwords)
   {
      m_keywords = std::move(kwords);
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
