#ifndef READER_WRITER_H
#define READER_WRITER_H

#include <iostream>
#include <functional>

typedef std::function<bool(char c)> reader_function;

class reader :
   public reader_function
{
public:

   reader(reader_function f) :
      reader_function(f)
   {
      std::cout << "reader()" << std::endl;
   }
   
   reader() : reader(
      [this] (char c) {
         return false;
      }
   )
   {
   }
   
   virtual void
   read(const std::string data)
   {
      for (char c : data)
         read(c);
   };
   
   virtual bool read(char c) {
      return (*this)(c);
   }
   
};

class either_reader :
   public reader
{
public:

   either_reader(
      reader_function f1,
      reader_function f2
   ) : f1_(f1), f1_alive_(true),
       f2_(f2), f2_alive_(true)
   {
   
   }
   
   virtual bool read(char c) {
   
      if (f1_alive_ && !f1_(c))
         f1_alive_ = false;
         
      if (f2_alive_ && !f2_(c))
         f2_alive_ = false;
         
      return (f1_alive_ || f2_alive_);
   }

protected:
   reader_function f1_; bool f1_alive_;
   reader_function f2_; bool f2_alive_;
};

class next_reader :
   public either_reader
{

public: 
   next_reader(reader_function f,
               reader_function next_f)
      : either_reader(f, next_f)
   {
   }
   
   virtual bool read(char c) {
      if (f1_alive_) {
         f1_alive_ = f1_(c);
         if (f1_alive_)
            return true;
      }
      f2_alive_ = f2_(c);
      return f2_alive_;
   }
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