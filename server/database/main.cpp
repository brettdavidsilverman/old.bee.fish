#include <stdio.h>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/chrono.hpp>
#include "database.h"
#include "pointer.h"
//Database database("db.b2");
using namespace boost::chrono;
using namespace bee::fish::database;

bool hasArg(
   int argc,
   const char* argv[],
   const std::string arg
);

int main(int argc, const char* argv[]) {

   clog << __cplusplus << endl;
   
   Database database("data");
   
   Pointer pointer(database);
   Pointer start(pointer);
   ReadOnlyPointer readOnlyPointer(pointer);
   
   
   bool readOnly =
      hasArg(argc, argv, "-r");
   
   if (readOnly)
   {
      clog << "Read only" << endl;
   }
   
   Pointer& p = 
      readOnly ?
         readOnlyPointer :
         pointer;

   clog << database;

   // Launch the pool with 1 thread
   int threadCount = 1;
   if (argc > 1 && !readOnly)
     threadCount = atoi(argv[1]);
   
   clog << "Threads: " << threadCount << endl;
   boost::asio::thread_pool threadPool(threadCount); 

   string line;
   long count = 0;

   auto startTime = system_clock::now();
   while (!cin.eof()) {
   
      getline(cin, line);
      
      if (line.length() == 0)
         break;
         
      p = {0, 0};
      
#ifdef DEBUG
      cerr << *p << line;
#endif

      p << line;
      
#ifdef DEBUG
      cerr << *p << endl;
#endif
      
      /*
      else 
      {
         boost::asio::dispatch(
            threadPool,
            [&database, line, &pointer]() {
               ReadOnlyPointer p(pointer);
               p << line;
            }
         );
      }
      */
      /*
      if (++count % 1000 == 0)
      {
         double time = 
           (
              system_clock::now() - 
              startTime
           ).count();
           
         if (p->_bitCount > 0)
         {
            double avg =
               time / (double)(p->_bitCount);
         
            if (avg > 0)
               cout << count
                    << '\t'
                    << avg
                    << endl;
         }
         
         start = system_clock::now();
         p->_bitCount = 0;
      }
      */
      
   }
  
   
   threadPool.join();
   
   clog << "ok:" << database << endl;

}

bool hasArg(
   int argc,
   const char* argv[],
   const std::string arg
)
{
   for (int i = 0; i < argc; i++)
   {
      if (arg == argv[i])
         return true;
   }
   
   return false;
}

