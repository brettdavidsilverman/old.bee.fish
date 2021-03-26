#include <stdio.h>
#include <iostream>
#include <chrono>
#include <sstream>

#include "database.h"
#include "path.h"
//#include "binary-encoding.h"

using namespace bee::fish::database;
using namespace std;

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
);

template<class Encoding>
void suggest(Path<Encoding> path, const BString& line);

void timer();

auto startTime =
   std::chrono::system_clock::now();
   
unsigned long _count = 0;


int main(int argc, const char* argv[]) {

   cerr << __cplusplus << endl;
 
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
   
   while (!cin.eof()) {
      
      getline(cin, line);
      
      if (line.size() == 0)
         break;
      
      try
      {
         path = root;
         if (read)
         {
            if (path.contains(line))
            {
               path << line;
               unsigned long* lineNo =
                  (unsigned long*)(path.getData());
                  
               if (lineNo)
                  cerr << *lineNo << endl;
            }
            else
            {
               suggest(path, line);
            }
         }
         else
         {
            path << line;
            path.setData(&_count, sizeof(_count));
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
     
      ++_count;
      
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

int hasArg(
   int argc,
   const char* argv[],
   const BString arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return i;
   }
   
   return -1;
}

template<class Encoding>
void suggest(Path<Encoding> path, const BString& line)
{

   Path match = path;
   BString start;
   
   match.readBit();
   for (char c : line)
   {
      Path test = match.contains(c);
      
      if (!test)
         break;
      match = test;
      start.push_back(c);
   }
   
   
   BString end;
   
   while (match.peekBit())
   {
      char c;
      match >> c;
      end.push_back(c);
   }
   
   match.readBit();
   
   BString suggested = start + end;
   
   cerr << suggested << endl;
}

