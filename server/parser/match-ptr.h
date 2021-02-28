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

   class MatchPtr : public MatchPtrBase
   {
   public:
   
      MatchPtr()
      {
      }
      
      MatchPtr(Match* match) :
         MatchPtrBase(match)
      {
      }
           
      MatchPtr(const Match* match) :
         MatchPtrBase(match->copy())
      {
      }
      
      MatchPtr(const Match& match) :
         MatchPtr(match.copy())
      {
      }
       
      MatchPtr& operator= (Match* assign)
      {
         MatchPtrBase::operator = (
            MatchPtrBase(assign)
         );
         return *this;
      }
      
   /*
      Match* _pointer;
      bool _owns;
      
      MatchPtr()
      {
         cerr << "MatchPtr()" << endl;
         
         _pointer = NULL;
         _owns = false;
      }
      
      MatchPtr(Match* pointer)
      {
         cerr << "MatchPtr(Match*)" << endl;
         _pointer = pointer;
         _owns = false;
      }
      
      MatchPtr(const Match& match)
      {
         cerr << "MatchPtr(const Match&)" << endl;
         _pointer = match.copy();
         _owns = true;
      }
      
      MatchPtr(const MatchPtr& source)
      {
         cerr << "MatchPtr(const MatchPtr&)" << endl;
         _pointer = source._pointer;
         _owns = false;
      }
      
      
      ~MatchPtr()
      {
         cerr << "~MatchPtr()" << endl;
         reset();
      }
      
      void reset()
      {
         if (_owns && _pointer) 
         {
            cerr << "delete" << endl;
            delete _pointer;
         }
         
         _pointer = NULL;
      }

      Match& operator*()
      {
         return *_pointer;
      }
      
      const Match& operator*() const
      {
         return *_pointer;
      }
      
      Match* operator ->()
      {
         return _pointer;
      }
      
      operator bool () const
      {
         return _pointer != NULL;
      }
      
      bool operator ! () const
      {
         return !_pointer;
      }
      
      friend ostream& operator << (ostream& out, MatchPtr pointer)
      {
         if (pointer)
            out << pointer._pointer;
         else
            out << "NULL";
            
         return out;
      }
      
      */
   };
   
   
   #define Pointer(x) MatchPtr(new x)

}

#endif