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
         const Size initialSize
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
               "Couldn't create database " +
               filePath
            );
         }
   
         resize(
            initialSize,
            fileno(file)
         );
   
         fclose(file);

      }

      void openFile() {
         // Open the file
         _file = fopen(
            filePath.c_str(), "r+"
         );
      
         if (_file == NULL) {
            throw runtime_error(
               "Couldnt open database " +
               filePath
            );
         }
      
         _fileNumber = fileno(_file);
         _size = getFileSize();
      }

      Size resize(const Size newSize)
      {

         return resize(
            newSize, _fileNumber
         );
   
      }
      
      int _fileNumber = -1;
      Size _size = -1;
   private:
      Size resize(
         const Size newSize,
         int fileNumber)
      {

         int result = ftruncate(
            fileNumber,
            newSize
         );
         
         if (result != 0) {
            throw runtime_error(
               "Error resizing database"
            );
         }
   
         _size = getFileSize();
   
         return _size;
   
      }
      
   private:
      FILE* _file = NULL;
      bool _isNew;
      
      Size getFileSize()
      {
         struct stat buffer;
         stat(filePath.c_str(), &buffer);
         return buffer.st_size;
      }
   };

   typedef File::Size Size;
   
}

#endif
