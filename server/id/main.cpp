#include <stdio.h>
#include <iostream>
#include <fstream>
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
   /*
   wifstream file("temp.txt");
   wstring key;
   getline(file, key);
   Id id(key);
   file.close();
   cout << id << endl;
   */
   Id id;
   cout << id << endl;
   
   
   cout << "Enter keys" << endl;
   
   while (!cin.eof())
   {
      string key;
      getline(cin, key);
     
      if (key == "")
         break;
        
      Id id(key);
     
      cout << id << endl;
      cout << id.hex() << endl;
   }
  
   cout << "Finished" << endl;
   
  
   /*
   wofstream file("temp.txt");
   
   vector<Id> array(1000);
   for (Id& id : array)
   {
     // wcout << id.key() << endl;
      cout << id << endl;
      file << id.key() << endl;
   }
   
   file.close();
   */
   
   return 0;
}
