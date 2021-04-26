

#include "loraine/grants/grantmodifier.h"

//bool out_of_the_way_filter(Grant& g)
//{
//   if(g.get_grant_type() == GrantType::KEYWORD) {
//      auto kg = dynamic_cast< KeywordGrant& >(g);
//      if(kg.get_keyword() == Keyword::BARRIER) {
//         // if the given keyword is BARRIER, then it won't be made permanent.
//         return false;
//      }
//   }
//   return true;
//}