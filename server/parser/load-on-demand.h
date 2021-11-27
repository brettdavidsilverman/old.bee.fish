#ifndef BEE_FISH_PARSER__LOAD_ON_DEMAMD_H
#define BEE_FISH_PARSER__LOAD_ON_DEMAMD_H

#include "match.h"

namespace BeeFishParser
{

	template<class T>
	class LoadOnDemand : public Match
	{
	public:
		T* _match;

	public:
		LoadOnDemand() : _match(nullptr)
		{
		}

		~LoadOnDemand() {
			if (_match)
				delete _match;
		}

		virtual void setup()
		{
			if (!_match)
				_match = new T();
			Match::setup();
		}

		virtual bool match(const Char& character) {
			bool matched = _match->matchCharacter(character);
			if (_match->_result == true)
				success();
			else if (_match->_result == false)
				fail();

			return matched;
		}
	};

}

#endif