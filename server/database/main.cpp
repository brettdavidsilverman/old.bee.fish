#include <stdio.h>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include <atomic>
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
   
   string fileName = "data";
   
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
      Database database(fileName);
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
      db = new Database(fileName);
      databases.push_back(db);
   }
   db = databases[0];
 
   clog << "Threads: " << threadCount << endl;
   boost::asio::thread_pool
      threadPool(threadCount); 

   string line;
   long count = 0;
   atomic<long> success = 0;
   mutex mtx;
   
   Path path(*db);
   
   cerr << *db;
   
   auto startTime = system_clock::now();
   while (!cin.eof()) {
   
      getline(cin, line);
      
      if (line.length() == 0)
         break;
      
      try
      {
         if (threadCount == 1)
         {
            path = Branch::Root;
            if (readOnly)
            {
               if (! path.contains(line) )
                  cerr << line << endl;
            }
            else
            {
               path << line;
            }
            
         }
         else
         {
            boost::asio::dispatch(
               threadPool,
               [
                  line,
                  &databases,
                  &mtx,
                  readOnly
               ] ()
               {
               
                 // cerr << line << endl;
                  
                  Database* db;
                  try
                  {
                     mtx.lock();
                  
                     db = databases.back();
                     databases.pop_back();
                  
                     mtx.unlock();
                  
                     Path path(*db);
                     if (readOnly)
                     {
                        if (! path.contains(line) )
                           cerr << line << endl;
                     }
                     else
                        path << line;
                  }
                  catch (exception err)
                  {
                     cerr << err.what() << endl;
                  }
                  catch (...)
                  {
                     cerr << "***Error***" << endl;
                  }
                 
                  mtx.lock();
                  databases.push_back(db);
                  mtx.unlock();
                 
               }
            );
         }
         
         //cerr << line << endl;
         ++success;
      }
      catch (exception err)
      {
         cerr << line << ':'
              << err.what()
              << endl;
        // throw err;
      }
      catch (...)
      {
         cerr << line << ':'
              << "****"
              << endl;
      //   throw runtime_error(line);
      }
     
      if (++count % 1000 == 0)
      {
         milliseconds ms =
         
         duration_cast <milliseconds>(
            (
               system_clock::now() -
               startTime
            )
         );
         
         cerr << count
              << '\t'
              << ms.count()
              << endl;
              
         startTime = system_clock::now();
      }
      
      
   }
   
   cerr << "Wait for threads....";
   
   threadPool.join();
   
   cerr << "joined" << endl;
   
   for (auto db : databases)
   {
      cerr << "********" << endl;
      
      cerr << *db;
      
      delete db;
   }
   
   cerr << "********" << endl;
   cerr << success << '/' << count;
   
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

