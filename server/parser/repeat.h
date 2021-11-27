#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include <vector>
#include "match.h"
namespace BeeFishParser
{

	template<class T>
	class Repeat : public Match
	{
	private:
		T *_template;
		vector<T *> _items;

	public:
		size_t _minimum = 1;
		size_t _maximum = 0;
		size_t _matchedCount = 0;

	public:
		Repeat(
			size_t minimum = 1,
			size_t maximum = 0) :
				_template(new T()),
				_minimum(minimum),
				_maximum(maximum)
		{
		}

		virtual ~Repeat()
		{
			delete _template;

			for (Match *item : _items)
				delete item;
		}

		virtual bool match(const Char &character)
		{

			bool matched =
				_template->matchCharacter(character);

			if (_template->_result == true)
			{

				matchedItem(_template);

				_template = new T();

				++_matchedCount;

				if (_maximum > 0 &&
					_matchedCount > _maximum)
				{
					matched = false;
					fail();
				}

				if (_matchedCount == _maximum)
					success();

			}
			else if (
				(_template->_result == false) ||
				(!matched))
			{
				if (_matchedCount >= _minimum)
				{
					success();
				}
				else
				{
					matched = false;
					fail();
				}
			}

			return matched;
		}

		virtual void matchedItem(T *match)
		{
			delete match;
		}

	};

};

#endif
