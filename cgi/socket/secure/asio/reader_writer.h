#ifndef READER_WRITER_H
#define READER_WRITER_H

#include <iostream>
#include <functional>

typedef std::function<bool(char c)> reader_function;

class reader :
   public reader_function
{
public:

   reader(
      reader_function f
   ) :
      reader_function(f)
   {
      std::cout << "reader()" << std::endl;
   }
   
   reader() :
      reader_function(
         [this](char c) {
            return false;
         }
      )
   {
   }
   
   bool operator ()(const std::string s) {
      reader_function& rf = *this;
      for (char c : s) {
         if (!rf(c))
            return false;
      }
      return true;
   }
   
};

class either_reader :
   public reader
{
public:

   either_reader(
      reader_function f1,
      reader_function f2
   ) :
      f1_(f1), f1_alive_(true),
      f2_(f2), f2_alive_(true),
      reader(
         [this] (char c) {
            std::cout << "er: " << c << std::endl;
            if (f1_alive_) {
               std::cout << "f1_" << c << std::endl;
               f1_alive_ = f1_(c);
            }
            
            if (f2_alive_) {
               std::cout << "f2_" << c << std::endl;
               f2_alive_ = f2_(c);
            }
            return (f1_alive_ || f2_alive_);
         }
      )
   {
      std::cout << "either_reader()" << std::endl;
   }

protected:
   reader_function f1_; bool f1_alive_;
   reader_function f2_; bool f2_alive_;
};

class sequenced_reader :
   public either_reader
{

public: 
   sequenced_reader(
      reader_function f,
      reader_function next_f
   ) : 
      either_reader(f, next_f)
   {
   }
   
   virtual bool read(char c) {
      std::cout << "sr:" << c << std::endl;
      if (f1_alive_) {
         f1_alive_ = f1_(c);
         if (f1_alive_)
            return true;
      }
      if (f2_alive_) {
         f2_alive_ = f2_(c);
         return f2_alive_;
      }
      return false;
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