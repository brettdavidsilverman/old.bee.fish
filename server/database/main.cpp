#include <stdio.h>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include "database.h"
#include "pointer.h"
//Database database("db.b2");
using namespace bee::fish::database;

int main(int argc, const char* argv[]) {
   clog << __cplusplus << endl;
   
   Database database("db2.data");
   
   Pointer pointer(&database, 0);
   ReadOnlyPointer readPointer(pointer);
   Pointer* p = &pointer;
   cout << database;
   
   bool readOnly = false;
   
   if (argc > 1)
   {
      cout << "Read only" << endl;
      p = &readPointer;
   }
   

   cout << database;

   // Launch the pool with 5 threads
   int threadCount = 5;
   if (argc > 1)
     threadCount = atoi(argv[1]);
   
   cout << "Threads: " << threadCount << endl;
   boost::asio::thread_pool threadPool(threadCount); 

   string line;
   while (!cin.eof()) {
   
      getline(cin, line);
      if (line.length() == 0)
         break;
      
#ifdef DEBUG
      cout << line << endl;
#endif

      *p = 0;
      *p << line;
      
#ifdef DEBUG
      cout << **p << endl;
#endif
      /*
      boost::asio::dispatch(
         threadPool,
         [&database, &line, &pointer]() {
            Pointer p(pointer);
            p << line;
         }
      );
      */
   }
  
   
   threadPool.join();
   
   cout << "ok:" << database << endl;

}
