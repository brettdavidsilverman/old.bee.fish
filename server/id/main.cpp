#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <locale>
#include "id.h"
#include "test.h"


using namespace std;
using namespace BeeFishId;

int main(int argc, const char* argv[]) {


   if (hasArg(argc, argv, "-test") >= 0)
   {
      cout << "Testing Id..." << endl;
      if (!BeeFishId::test())
         return 1;
            
      return 0;
   }
   
   while (!cin.eof())
   {
      BString line;
      std::cout << "Name: ";
      getline(cin, line);

      if (line == "")
         break;

      Id id(line);

      cout << id.toString() << endl;
      cout << id.key() << endl;
   }

   cout << "Finished" << endl;

/*

   ofstream ofile("temp.txt");
   vector<Id> array(1000);
   for (Id& id : array)
   {
      cout << id.key() << endl;
      ofile << id.key() << endl;
   }

   ofile << endl;

   ofile.close();


   ifstream ifile("temp.txt");
   string key;
   for (;;) {
      getline(ifile, key);
      if (key == "")
         break;
      Id id = Id::fromKey(key);
      cout << id.toString() << endl;
   }

   ifile.close();
*/

   return 0;

}

