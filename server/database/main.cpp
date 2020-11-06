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
   
   vector<Database*> databases;
   Database* db = NULL;
   for (int i = 0; i < threadCount; ++i)
   {
      db = new Database(database);
      databases.push_back(db);
   }
   db = databases[0];
   
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
            Path path(*db);
            if (readOnly)
            {
               if (! path.contains(line) )
                  cerr << line << endl;
            }
            else
            {
               path << line;
            }
          //  cerr << 1 << endl;
         }
         else
         {
            boost::asio::dispatch(
               threadPool,
               [line, &databases, &lock, readOnly]() {
            
                  lock.lock();
               
                  Database* db = databases.back();
      
                  databases.pop_back();
       
                  lock.unlock();
         
                  Path path(*db);
                  
                  if (readOnly)
                  {
                     if (! path.contains(line) )
                        cerr << line << endl;
                  }
                  else
                     path << line;
                  
                  lock.lock();
                  databases.push_back(db);
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
   
   for (auto db : databases)
   {

      delete db;
   }
   
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

