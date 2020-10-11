#ifndef BEE_FISH_DATABASE__MAP_H
#define BEE_FISH_DATABASE__MAP_H

#include <stdexcept>
#include "pointer.h"


using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

   template<class Key, class T>
   class Map :
      public Pointer
   {


   public:
   
      Map( Database& database,
               Index index = Index::root ) :
         Pointer(database, index)
      {
         (*this) << "Map:"
                 << typeid(Key).hash_code()
                 << ":"
                 << typeid(T).hash_code();
         _start = *(*this);
      }
      
      T& operator [] (const Key& key)
      {
         *(this) = _start;
         *(this) << key;
         if (isDeadEnd())
            throw runtime_error("Dead end");
         Pointer start(*this);
         return T(start);
      }
   
   protected:
      Index _start;

   };



}

#endif