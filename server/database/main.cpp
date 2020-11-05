#include <stdio.h>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include <chrono>
#include "database.h"
#include "path.h"
#include "map.h"

using namespace std::chrono;
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
   
   /*
   cerr << "true, false" << endl;
   path << true << false;
   cerr << endl;
   
   cerr << "true, true" << endl;
   path = start;
   path << true << true;
   cerr << endl;
   
   cerr << "Path index: ";
   cerr << *path;
   cerr << endl;
   path = start;
   
   cerr << "Traverse " << endl;
   cerr << path;
   cerr << endl;
   return 0;
   */
   
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
      Path path(database);
      cout << path;
      return 0;
   }
   
   // Launch the pool with 1 thread
   int threadCount = 1;
   
   int argThreadCount =
      hasArg(argc, argv, "-threads") + 1;
   
   if (argThreadCount > 0 && 
       argc > argThreadCount)
     threadCount = atoi(argv[argThreadCount]);
   
   vector<Path*> paths;
   Path* p = NULL;
   for (int i = 0; i < threadCount; ++i)
   {
      if (readOnly)
         p = new ReadOnlyPath(database);
      else
         p = new Path(database);
      paths.push_back(p);
   }
   p = paths[0];
   
   clog << "Threads: " << threadCount << endl;
   boost::asio::thread_pool threadPool(threadCount); 

   string line;
   long count = 0;
   mutex lock;
   
   
   auto startTime = system_clock::now();
   while (!cin.eof()) {
   
      getline(cin, line);
      
      if (line.length() == 0)
         break;
      
      try
      {
         if (threadCount == 1)
         {
          // cerr << line;
            *p = Branch::Root;
            (*p)["Brett"] << line;
          //  cerr << 1 << endl;
         }
         else
         {
            boost::asio::dispatch(
               threadPool,
               [line, &paths, &lock]() {
            
                  lock.lock();
               
                  Path* p = paths.back();
      
                  paths.pop_back();
       
                  lock.unlock();
         
                  *p = Branch::Root;
                  
                  try
                  {
                     *p << line;
                  }
                  catch (runtime_error err)
                  {
                     cerr << line << ':'
                          << err.what()
                          << endl;
                  }
                  
                  lock.lock();
                  paths.push_back(p);
                  lock.unlock();
               }
            );
         }
      }
      catch (runtime_error err)
      {
         cerr << line << ':'
              << err.what()
              << endl;
      }
      catch (...)
      {
         cerr << line << ':'
              << "****"
              << endl;
      }
      
      
      
      /*
      if (++count % 10000 == 0)
      {
         milliseconds ms =
         
         duration_cast <milliseconds>(
            (
               system_clock::now() -
               startTime
            )
         );
         
         cout << count
              << '\t'
              << ms.count()
              << endl;
              
         startTime = system_clock::now();
      }
      */
      
   }
   
   threadPool.join();
   
   for (auto path : paths)
      delete path;
      
   cerr << database;
   
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

