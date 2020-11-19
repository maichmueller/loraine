
#include "target.h"


sptr< NexusTarget > to_nexus_target(const sptr< BaseTarget >& target)
{
   return std::dynamic_pointer_cast< NexusTarget >(target);
}
sptr< CardTarget > to_card_target(const sptr< BaseTarget >& target)
{
   return std::dynamic_pointer_cast< CardTarget >(target);
}