#ifndef BEE_FISH_PARSER__MATCH_PTR_H
#define BEE_FISH_PARSER__MATCH_PTR_H

#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include "version.h"

#include "match.h"

using namespace std;

namespace bee::fish::parser {

   
   class MatchPointerBase
   {
   public:
      Match* _pointer;
      
      MatchPointerBase()
      {
         _pointer = nullptr;
      }
      
      MatchPointerBase(Match* pointer)
      {
         _pointer = pointer;
      }
      
      MatchPointerBase(const Match& match)
      {
         _pointer = match.copy();
      }
      
      MatchPointerBase(MatchPointerBase& pointer)
      {
         _pointer = pointer._pointer;
      }
   
      Match* get()
      {
         return _pointer;
      }
      
      Match* operator ->() const
      {
         return _pointer;
      }
      
      operator bool() const
      {
         return _pointer != nullptr;
      }

      Match& operator *() const
      {
         return *_pointer;
      }
      
      operator Match*() const
      {
         return _pointer;
      }
      
      Match* copy() const
      {
         return _pointer->copy();
      }
      
      void operator delete (void* pointer)
      {
         MatchPointerBase* ptr =
            static_cast<MatchPointerBase*>(pointer);
         delete ptr->get();
         ::operator delete(ptr);
      }
   };

      
   template<class T = Match>
   class MatchPointer :
      public MatchPointerBase
   {
   public:

      MatchPointer() :
         MatchPointerBase()
      {
      }
      
      MatchPointer(T* pointer) :
         MatchPointerBase(pointer)
      {
      }
      
      
      MatchPointer(MatchPointer& pointer) :
         MatchPointerBase(pointer)
      {
      }

      MatchPointer(const T& match) :
         MatchPointerBase(match)
      {
      }
      
      MatchPointer(MatchPointerBase& pointer) :
         MatchPointerBase(pointer)
      {
      }
     
      T* operator ->() const
      {
         return (T*)_pointer;
      }
      
      T& operator *() const
      {
         return *(T*)_pointer;
      }
      
      
   };
   
#define P(x) MatchPointer(new x)

}

#endif