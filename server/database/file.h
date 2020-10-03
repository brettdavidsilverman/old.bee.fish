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
         const string& path,
         const Size initialSize = 0
      ) : filePath(path), _file(NULL)
      {
         // Create the file if it
         // doesnt exist
         if (fileExists() == false) {
            createFile(initialSize);
            _isNew = true;
         }
         else
            _isNew = false;
      
         openFile();

      }

      ~File() {

         if (_file) {
            fclose(_file);
            _file = NULL;
         }
   
      }

   
      const string filePath;
      
      Size fileSize() const
      {
         return _size;
      }

      bool isNew() const
      {
         return _isNew;
      }
      
      Size seek
      (
         Size offset,
         int origin = SEEK_SET
      )
      {
         size_t result =
            fseek(
               _file,
               offset,
               origin
            );
            
         if (result != 0)
         {
            string error =
               std::strerror(errno);
            error =
               "Error seeking file." +
               error;
            throw runtime_error(
               error
            );
         }
         
         return offset;
      }
      
      Size read
      (
         void * ptr,
         size_t count,
         size_t size
      )
      {
         size_t result =
            fread(
               ptr,
               count,
               size,
               _file
            );

         if (result != (count * size))
         {
            string error =
               std::strerror(errno);
            error =
               "Error reading file." +
               error;
            throw runtime_error(
               error
            );
         }
         
         return result;
      }
      
      
      Size write
      (
         const void * ptr,
         size_t count,
         size_t size
      )
      {
         size_t result =
            fwrite(
               ptr,
               count,
               size,
               _file
            );

         if (result != (count * size))
         {
            string error =
               std::strerror(errno);
            error =
               "Error writing file." +
               error;
            throw runtime_error(
               error
            );
         }
         
         return result;
      }
   
   protected:

      bool fileExists()
      {
         return (
            access(
               filePath.c_str(),
               F_OK
            ) == 0
         );
      }

      void createFile(const Size initialSize)
      {
         FILE* file = fopen(
            filePath.c_str(), "w+"
         );
         
         if (file == NULL) {
            throw runtime_error(
               "Couldn't create file " +
               filePath
            );
         }
   
         resize(
            fileno(file),
            initialSize
         );
   
         fclose(file);

      }

      void openFile() {
         // Open the file
         _file = fopen(
            filePath.c_str(), "rb+"
         );
      
         if (_file == NULL) {
            throw runtime_error(
               "Couldnt open file " +
               filePath
            );
         }
      /*
         if ( setvbuf(
                 _file,
                 NULL,
                 0,
                 _IONBF) != 0 )
         {
            string error =
               std::strerror(errno);
            error =
               "Error setting file buffer." +
               error;
            throw runtime_error(
               error
            );
         }
         */
         _fileNumber = fileno(_file);
         _size = getFileSize(_fileNumber);
      }

      Size resize(const Size newSize)
      {

         resize(
            _fileNumber,
            newSize
         );
         
         _size = getFileSize(_fileNumber);
         
         return _size;
   
      }
      
      int _fileNumber = -1;
      Size _size = -1;
      
   private:
      static void resize(
         int fileNumber,
         int newSize
      )
      {
         int result =
            ftruncate(fileNumber, newSize);
    
         if (result != 0)
            throw runtime_error("Couldn't resize file.");
      }
      
   private:
      FILE* _file = NULL;
      bool _isNew;
      
      static Size getFileSize(int file)
      {
         struct stat buffer;
         fstat(file, &buffer);
         return buffer.st_size;
      }
   };

   typedef File::Size Size;
   
}

#endif
