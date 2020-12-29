#include <stdio.h>
#include <iostream>
#include <vector>
#include <locale>
#include "id.h"

void init_locale(void)
{

   // The correct locale name may vary by OS, e.g., "en_US.utf8".
   constexpr char locale_name[] = "";
   setlocale( LC_ALL, locale_name );
   std::locale::global(std::locale(locale_name));
   std::wcin.imbue(std::locale());

   std::wcout.imbue(std::locale());

}

using namespace std;
using namespace bee::fish::server;

int main(int argc, const char* argv[]) {
   init_locale();
   vector<Id> array(100);
   for (Id& id : array)
   {
      wcout << id.key() << endl;
   }
   
}