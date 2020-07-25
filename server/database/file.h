#ifndef BEE_FISH_DATABASE__FILE_H
#define BEE_FISH_DATABASE__FILE_H

#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>

using namespace std;

namespace bee::fish::database {

   class File {
   public:
      typedef unsigned long long Size;
   
   public:
      File(
         const string filePath,
         const Size initialSize
      );
      ~File();
   
      const string filePath;
      Size fileSize() const;
      bool isNew();
   
   protected:

      bool fileExists();
      void createFile(const Size initialSize);
      void openFile();

      virtual Size resize(Size newSize);
      int _fileNumber;
      Size _size = -1;
   private:
      Size resize(Size newSize, int fileNumber);
   
   private:
      FILE* _file = NULL;
      bool _isNew;
      Size getFileSize();
   };

   typedef File::Size Size;
   
}

#endif
