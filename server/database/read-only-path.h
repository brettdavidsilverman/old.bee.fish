#ifndef BEE_FISH_DATABASE__READ_ONLY_PATH_H
#define BEE_FISH_DATABASE__READ_ONLY_PATH_H

#include "path.h"

using namespace std;
using namespace bee::fish::power_encoding;

namespace bee::fish::database
{

   class ReadOnlyPath :
      public Path
   {

      
   public:
         
      ReadOnlyPath(
         Database& database,
         Index index = Branch::Root 
      ) :
         Path(database, index)
      {
      }
      
      ReadOnlyPath(const Path& source)
         : Path(source)
      {
      }
         
      virtual void writeBit(bool bit)
      {
         if (isDeadEnd())
            return;
            
         Branch& branch =
            _database.getBranch(_index);
            
         if (!bit && branch._left)
            _index = branch._left;
         else if (bit && branch._right)
            _index = branch._right;
         else
            throw runtime_error("Past dead end");
            
      }
      
   };
   
};

#endif