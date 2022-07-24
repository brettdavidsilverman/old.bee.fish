#include <stdio.h>

#include <iostream>

#include <fstream>

#include <vector>

#include <locale>

#include "id.h"

#include "test.h"


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

using namespace BeeFishId;


int main(int argc, const char* argv[]) {

  // init_locale();

   /*

   wifstream file("temp.txt");

   wstring key;

   getline(file, key);

   Id id(key);

   file.close();

   wcout << id << endl;

   */

   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing Id..." << endl;
      if (!BeeFishId::test())
         return 1;
            
      return 0;
   }
   
   Id id("Test");
   
   cout << id.toString() << endl;

   BString key = id.key();
   
   cout << key << endl;
   
   Id id2 = Id::fromKey(key);

   cout << id2.toString() << endl;

   cout << id << endl;

   cout << "Enter keys" << endl;

   

   while (!cin.eof())

   {

      BString line;

      getline(cin, line);

     

      if (line == "")

         break;

        

      Id id = Id::fromKey(line);

     

      cout << id << endl;

      cout << id.toString() << endl;

      cout << id.key() << endl;

   }

  

   cout << "Finished" << endl;

   

  

   /*

   wofstream file("temp.txt");

   

   vector<Id> array(1000);

   for (Id& id : array)

   {

     // wcout << id.key() << endl;

      wcout << id << endl;

      file << id.key() << endl;

   }

   

   file.close();

   */

   

   return 0;

}
