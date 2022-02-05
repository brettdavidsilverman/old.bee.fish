#include "bee-fish.h"

namespace BeeFishParser {

    const BString Match::EmptyString = {};
    unsigned long Match::_matchInstanceCount = 0;

}

namespace BeeFishId
{
    
    unsigned long Timestamp::_lastMs = 0;
    unsigned long Timestamp::_lastInc = 0;

}