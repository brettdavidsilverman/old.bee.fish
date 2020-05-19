#ifndef READER_WRITER_H
#define READER_WRITER_H

#include <iostream>
#include <functional>

typedef std::function<void(char c)> reader_function;

class reader
{
public:
   reader()
   {
      std::cout << "reader()" << std::endl;
   }
   
   virtual void
   read(const std::string data)
   {
      for (char c : data)
         read(c);
   };
   
   virtual void read(char c) = 0;
   
};

class reader_writer :
   public reader
{
public:
  // typedef void (*callback)(std::string data);
   
   reader_writer() {
   }
  

};

#endif