#include <stdio.h>
#include <iostream>
#include <chrono>

#include "database.h"
#include "path.h"
#include "string-encoding.h"
#include "binary-encoding.h"

using namespace bee::fish::database;

int hasArg(
   int argc,
   const char* argv[],
   const std::string arg
);
auto startTime =
   std::chrono::system_clock::now();
long _count = 0;
void timer();

int main(int argc, const char* argv[]) {

   clog << __cplusplus << endl;

   /*
   PowerEncoding enc;
   enc << true; cout << endl;
   enc << false; cout << endl;
   
   string fileName = "data";
   
   Database database(fileName);
   Path path(database);
   
   cerr << "true, false" << endl;
   path << true << false;
   cerr << endl;
   
   cerr << "true, true" << endl;
   path = 0;
   path << true << true;
   cerr << endl;
   
   cerr << "Path index: ";
   cerr << *path;
   cerr << endl;
   path = 0;
   
   cerr << "Traverse " << endl;
   cerr << path;
   cerr << endl;
   return 0;
   
   */
   
   bool contains =
      (hasArg(argc, argv, "-contains") != -1);
   
   if (contains)
   {
      clog << "Contains" << endl;
   }
   
   string fileName = "data";
   
   Database database(fileName);
   cerr << database;
   Path<PowerEncoding> root(database);
   Path path(root);
   bool output =
      (hasArg(argc, argv, "-output") != -1);
      
   if (output)
   {
      cout << path;
      return 0;
   }
   
   bool input =
      (hasArg(argc, argv, "-input") != -1);
      
   if (input)
   {
      cin >> path;
      return 0;
   }
 
   string line;
   long success = 0;
   
   while (!cin.eof()) {
   
      getline(cin, line);
      
      if (line.length() == 0)
         break;
      
      try
      {
         path = root;
         if (contains)
         {
            if (!path.contains(line))
               cerr << line << endl;
         }
         else
         {
            path << line;
         }
        // timer();
        // ++success;
      }
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
     

      
   }
   
   cerr << database;
   
   cerr << "********" << endl;
   cerr << success << '/' << _count;
   
   
   
}

void timer()
{

   if (++_count % 5000 == 0)
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

int hasArg(
   int argc,
   const char* argv[],
   const std::string arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return i;
   }
   
   return -1;
}

