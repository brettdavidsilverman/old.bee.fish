#include "file.h"

File::File(
   const string path,
   const Size initialSize
) :
   filePath(path)
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

File::~File() {

   std::cout << "~File" << std::endl;
   
   if (_file) {
      fclose(_file);
      _file = NULL;
   }
   
}

bool File::fileExists() {
   return (
      access(filePath.c_str(), F_OK) == 0
   );
}

Size
File::getFileSize() {
   struct stat buffer;
   stat(filePath.c_str(), &buffer);
   return buffer.st_size;
}

Size
File::fileSize() {
   return _size;
}

bool
File::isNew() {
   return _isNew;
}

Size File::resize(const Size newSize) {

   return resize(newSize, _fileNumber);
   
}

Size File::resize(const Size newSize, int fileNumber) {

   int result = ftruncate(
      fileNumber,
      newSize
   );
         
   if (result != 0) {
      perror("Truncating database");
      throw "Error resizing database";
   }
   
   _size = getFileSize();
   
   return _size;
   
}

void File::createFile(const Size initialSize) {
   FILE* file = fopen(
      filePath.c_str(), "w+"
   );
         
   if (file == NULL) {
      perror("Couldn't create database");
      throw "Couldn't create database " + filePath;
   }
   
   resize(
      initialSize,
      fileno(file)
   );
   
   fclose(file);

}
   
void File::openFile() {
   // Open the file
   _file = fopen(
      filePath.c_str(), "r+"
   );
      
   if (_file == NULL) {
      perror("Couldnt open database");
      throw "Couldnt open database " + filePath;
   }
      
   _fileNumber = fileno(_file);
   _size = getFileSize();
}

  