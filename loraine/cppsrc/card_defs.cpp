#include "cards/card_defs.h"

KeywordMap create_kword_list(std::initializer_list< Keyword > kwords)
{
   KeywordMap kword_list;
   std::fill(kword_list.begin(), kword_list.end(), false);
   for(auto kword : kwords) {
      kword_list[static_cast< unsigned long >(kword)] = true;
   }
   return kword_list;
}
