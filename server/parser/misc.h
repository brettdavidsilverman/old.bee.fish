#ifndef BEE_FISH_PARSER__MISC_H
#define BEE_FISH_PARSER__MISC_H

#include <ostream>
#include <optional>

using namespace std;

inline ostream& operator <<
(std::basic_ostream<char>& out, std::optional<bool> ok)
{
   if (ok == true)
      out << "true";
   else if (ok == false)
      out << "false";
   else
      out << "?";
         
   return out;
}

#endif  