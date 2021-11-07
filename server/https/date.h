#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <iomanip>
#include <ctime>

namespace date
{
   void writeDateTime(ostream& out)
   {
      auto t = std::time(nullptr);
      auto tm = *std::localtime(&t);
   
      out << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
   
   }
}


#endif