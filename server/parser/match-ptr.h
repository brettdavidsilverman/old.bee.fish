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

   
   typedef shared_ptr<Match> MatchPtrBase;
   
   class MatchPtr : public MatchPtrBase
   {
   public:
   
      MatchPtr()
      {
      }
      
      MatchPtr(MatchPtrBase match) :
         MatchPtrBase(match)
      {
      }
     
      
      MatchPtr(const MatchPtr& match) :
         MatchPtrBase(match)
      {
      }
      
      template<typename T>
      MatchPtr(shared_ptr<T> match) :
         MatchPtrBase(match)
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
         MatchPtrBase(match.copy())
      {

      }
 /*
      MatchPtr& operator= (Match* assign)
      {
         MatchPtrBase pointer(assign);
         
         MatchPtrBase::operator = (
            pointer
         );
         return *this;
      }
      */
      MatchPtr& operator= (MatchPtr assign)
      {
         MatchPtrBase::operator = (
            MatchPtrBase(assign)
         );
         return *this;
      }
      /*
      Match* operator ->() const
      {
         return MatchPtrBase::operator -> ();
      }
      
      Match& operator *() const
      {
         return MatchPtrBase::operator * ();
      }
      
      operator bool() const
      {
         return MatchPtrBase::operator bool();
      }
      */
   };
   

}

#endif