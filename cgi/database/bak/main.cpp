#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/thread_pool.hpp>
#include <boost/bind.hpp>
#include <boost/asio/thread_pool.hpp>
#include "database.h"


void invocationThread(
   Database& database,
   string& inputFile,
   boost::asio::thread_pool& threadPool
);

int readFromConsole(Database& database) {

   while (!cin.eof()) {
      string line;
      getline(cin, line);
      cout << line;
      if (line.length() == 0)
         break;
      Database::Pointer pointer =
         database.putString(0, line);
      cout << "\t" << pointer << endl;
   }
   
   cout << "----------------------" 
        << endl
        << "ok "
        << database.fileSize() 
        << endl;
        
   return 0;
}

int main(int argc, const char* argv[]) {
   std::mutex m;
   clog << __cplusplus << endl;

   Database database("db.b2");
   
   string inputFilePath = "shuffle.txt";
   int invocations = 1;
   int threadCount = 1;
   
   if (argc <= 1) {
      readFromConsole(database);
      return 0;
   }
   
   if (argc > 1)
      inputFilePath = argv[1];
      
   if (argc > 2)
      invocations = atoi(argv[2]);
      
   if (argc > 3)
      threadCount = atoi(argv[3]);
   
   cout << "input file path: " << inputFilePath << endl;
   cout << "invocations:     " << invocations << endl;
   cout << "thread count:    " << threadCount << endl;
   
   boost::asio::thread_pool threadPool(threadCount);
   
   Pointer initialPointer = database._data->nextPointer;
   
   cout << "database initial size:   " << database.fileSize() << endl;
   
   // Launch the thread pool
   
   boost::asio::thread_pool invocationThreadPool(invocations); 

   for (int i = 0; i < invocations; i++) {
      boost::asio::dispatch(
         invocationThreadPool,
         [&database, &inputFilePath, &threadPool]
         {
            invocationThread(
               database,
               inputFilePath,
               threadPool
            );
         }
      );
   }
   
   invocationThreadPool.join();
   
   threadPool.join();
   
   cout << "initial pointer: "
        << initialPointer
        << endl;
   cout << "final pointer:   "
        << database._data->nextPointer
        << endl;
        
   
}

void invocationThread(
   Database& database,
   string& inputFilePath,
   boost::asio::thread_pool& threadPool
) {
   
   ifstream input;
   input.open(inputFilePath, ios::in);
   while (!input.eof()) {
      string line;
      getline(input, line);
      
      if (line.length() == 0)
         break;
      
      boost::asio::dispatch(
         threadPool,
         [&database, line]() {
            Database::Pointer 
               bookmark =
                  database.putString(0, line);
            
         }
      );
      
      
      
   }
  
}
