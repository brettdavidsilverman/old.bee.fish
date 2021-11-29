#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser
{

	template<class T>
	class LoadOnDemand : public Match
	{

	public:
		LoadOnDemand() : Match()
		{
		}

		~LoadOnDemand() {
		}

		virtual void setup()
		{
			if (!_match)
				_match = new T();
			Match::setup();
		}

	};

}

#endif