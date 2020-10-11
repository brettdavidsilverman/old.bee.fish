#ifndef BEE_FISH_DATABASE__MAP_H
#define BEE_FISH_DATABASE__MAP_H

#include <stdexcept>
#include "pointer.h"
#include "../power-encoding/power-encoding.h"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database {

   template<class Key, class Value>
   class Map :
      public Pointer
   {
         
   public:
   
      Map( Database& database,
               Index index = Index::root ) :
         Pointer(database, index)
      {
         start();
      }
      
      Map(const Pointer pointer ) :
         Pointer(pointer)
      {
         start();
      }
      
      Value get(const Key& key)
      {
         _index = _start;
         
         *(this) << key;
         
         if (isDeadEnd())
            throw runtime_error("Dead end");
            
         Value value;
         (*this) >> value;

         return value;
      }
      
      Value operator [] (const Key& key)
      {
         return get(key);
      }
      
      void set(
         const Key& key,
         const Value& value
      )
      {
         _index = _start;
         
         *(this) << key;
         *(this) << value;
      }
      
      bool has(const Key& key)
      {
         _index = _start;
         
         *(this) << key;
         
         return (!isDeadEnd());
      }
   
   private:
      void start()
      {
         (*this) << "Map"
                 << typeid(Key).hash_code()
                 << typeid(Value).hash_code();
                 
         _start = *(*this);
      }

   protected:
      Index _start;

   };



}

#endif