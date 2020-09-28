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

int main(int argc, const char* argv[]) {
/*
   File test("test.txt");
   char data[4];
   memcpy(data, "abc", sizeof(data));
   cout << data << endl;
   test.write(data, 1, sizeof(data));
   test.seek(0, SEEK_SET);
   memset(data, '\0', sizeof(data));
   test.read(data, 1, sizeof(data));
   cout << data << endl;
   return 0;
   */
   clog << __cplusplus << endl;
   
   Database database("data");
   
   Pointer pointer(&database);
   Pointer start(pointer);
   ReadOnlyPointer readPointer(pointer);
   Pointer* p = &pointer;
   
   bool readOnly = false;
   
   if (argc == 1)
   {
      clog << "Read only" << endl;
      p = &readPointer;
   }
   

   clog << database;

   // Launch the pool with 1 thread
   int threadCount = 1;
   if (argc > 1)
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
         
      *p = {0, 0};
      
#ifdef DEBUG
      cerr << **p << line;
#endif

      *p << line;
      
#ifdef DEBUG
      cerr << **p << endl;
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
