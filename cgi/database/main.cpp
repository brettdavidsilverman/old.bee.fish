#include <stdio.h>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include "database.h"

//Database database("db.b2");

int main(int argc, const char* argv[]) {
   clog << __cplusplus << endl;
   
   Database database("db.b2");

   // Launch the pool with 5 threads
   int threadCount = 5;
   if (argc > 1)
     threadCount = atoi(argv[1]);
   
   cout << threadCount << endl;
   cout << getpagesize() << endl;
   boost::asio::thread_pool threadPool(threadCount); 

   string line;
   while (!cin.eof()) {
      getline(cin, line);
      if (line.length() == 0)
         break;
      //database.putString(0, line);
      
      boost::asio::dispatch(
         threadPool,
         [&database, line]() {
           // cerr << line << "...";
            Database::Pointer 
               bookmark = database.putString(0, line);
            //cerr << "ok (" << bookmark << ")" << endl;
         }
      );
      
   }
   
   threadPool.join();
   
   cout << "ok:" << database.fileSize() << endl;

}