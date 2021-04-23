

#ifndef LORAINE_YARD_HPP
#define LORAINE_YARD_HPP

class Yard {
  private:
   // all the cards played
   SymArr< std::map< size_t, std::vector< entt::entity > > > m_playyard = {};
   // all the dead unit cards
   SymArr< std::map< size_t, std::vector< entt::entity > > > m_graveyard = {};
   // all the used up spell cards
   SymArr< std::map< size_t, std::vector< entt::entity > > > m_spellyard = {};
   // all the tossed cards
   SymArr< std::map< size_t, std::vector< entt::entity > > > m_tossyard = {};
};

#endif  // LORAINE_YARD_HPP
