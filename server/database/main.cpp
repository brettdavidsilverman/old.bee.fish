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

using namespace boost::chrono;
using namespace bee::fish::database;

int hasArg(
   int argc,
   const char* argv[],
   const std::string arg
);

int main(int argc, const char* argv[]) {

   clog << __cplusplus << endl;
   
   Database database("data");
   
   clog << database;
   
   Pointer pointer(database);
   
   
   Pointer start(pointer);
   pointer = start;
   ReadOnlyPointer readOnlyPointer(pointer);
   
   bool readOnly =
      (hasArg(argc, argv, "-read") != -1);
   
   if (readOnly)
   {
      clog << "Read only" << endl;
   }
   
   bool traverse =
      (hasArg(argc, argv, "-traverse") != -1);
      
   if (traverse)
   {
      cout << pointer;
      cout << *pointer;
      return 0;
   }
   
   Pointer& p = 
      readOnly ?
         readOnlyPointer :
         pointer;

   // Launch the pool with 1 thread
   int threadCount = 1;
   /*
   int argThreadCount =
      hasArg(argc, argv, "-threads") + 1;
   
   if (argThreadCount > 0 && 
       argc > argThreadCount)
     threadCount = atoi(argv[argThreadCount]);
   */
   clog << "Threads: " << threadCount << endl;
   boost::asio::thread_pool threadPool(threadCount); 

   string line;
   long count = 0;

   auto startTime = system_clock::now();
   while (!cin.eof()) {
   
      getline(cin, line);
      
      if (line.length() == 0)
         break;
      
      if (threadCount == 1)
      {
         p = Index::root;
      
#ifdef DEBUG
         cerr << *p << line;
#endif

         p << line;
      
         
#ifdef DEBUG
         cerr << *p << endl;
#endif
      }
      else
      {
         boost::asio::dispatch(
            threadPool,
            [line, &start]() {
              // cerr << line << endl;
               ReadOnlyPointer pointer(start);
               pointer << line;
            }
         );
      }
      
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

