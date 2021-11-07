#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#ifdef SERVER
#include "database.h"
#include "path.h"
#include "test.h"

using namespace BeeFishDatabase;
using namespace std;


template<class Encoding>
void suggest(Path<Encoding> path, const BString& line);

void timer();

auto startTime =
   std::chrono::system_clock::now();
   
unsigned long _count = 0;


int main(int argc, const char* argv[]) {

   cerr << "bee.fish.database"
           << endl
        << "C++ run time: "
           << __cplusplus
           << endl
        << "Version: "
           << BEE_FISH_DATABASE_VERSION
           << endl;
 
   bool test =
      (hasArg(argc, argv, "-test") != -1);
   
   if (test)
   {
      if (BeeFishDatabase::test())
         return 0;
         
      return 1;
   }
   
   bool read =
      (hasArg(argc, argv, "-read") != -1);
   
   if (read)
   {
      cerr << "Read" << endl;
   }
   
   bool output =
      (hasArg(argc, argv, "-output") != -1);
      
   if (output)
   {
      cerr << "Output" << endl;
   }

   bool input =
      (hasArg(argc, argv, "-input") != -1);
      
   if (input)
   {
      cerr << "Input" << endl;
   }
   
   BString fileName = "data";
   
   Database database(fileName);
   cerr << database;
   Path<PowerEncoding> root(database);
   Path path(root);
   
   if (output)
   {
      cout << path;
      return 0;
   }
   
   if (input)
   {
      cin >> path;
      return 0;
   }
 
   BString line;
   long success = 0;
   unsigned long count = 0;
   while (!cin.eof()) {
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      //try
      {
         path = root;
         if (read)
         {
            if (path.contains(line))
            {
               path << line;
               //unsigned long lineNo;
              // path.getData(lineNo);
                  
               //cerr << lineNo << endl;
            }
            else
            {
               //suggest(path, line);
            }
         }
         else
         {
            path << line;
            //path.setData(count);
         }
        // timer();
        // ++success;
      }
      /*
      catch (exception err)
      {
         cerr << line << ':'
              << err.what()
              << endl;
         throw err;
      }
      catch (...)
      {
         cerr << line << ':'
              << "****"
              << endl;
         throw runtime_error(line);
      }
     */
     
     ++count;
      
   }
   
   cerr << database;
   
   
   
}

void timer()
{

   if (_count % 5000 == 0)
   {
      std::chrono::milliseconds ms =
         
         std::chrono::duration_cast
         <std::chrono::milliseconds>(
            (
               std::chrono::system_clock::now() -
               startTime
            )
         );
         
      cerr << _count
           << '\t'
           << ms.count()
           << endl;
         
      startTime =
         std::chrono::system_clock::now();
   }
      
}

/*
template<class Encoding>
void suggest(Path<Encoding> path, const BString& line)
{

   Path match = path;
   BString start;
   
   match.readBit();
   for (char c : line)
   {
      BeeFishBString::
         Character character(c);
      
      bool contains = match.contains(character);
      
      if (!contains)
         break;
         
      match = match[character];
      start.push_back(character);
   }
   
   
   BString end;
   
   while (match.peekBit())
   {
      BeeFishBString::
         Character character;
      match = match.next(character);
      end.push_back(character);
   }
   
   CHECK(match.readBit() == false);
   
   BString suggested = start + end;
   
   cerr << suggested << endl;
}

*/
#else
   int main(void) {
      std::cerr << "Database only runs on a server" << std::endl;
      return 0;
   }
#endif
