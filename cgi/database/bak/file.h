#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>

using namespace std;

class File {
public:
   typedef unsigned long Size;
   
public:
   File(
      const string filePath,
      const Size initialSize
   );
   ~File();
   
   const string filePath;
   Size fileSize();
   bool isNew();
   
protected:

   bool fileExists();
   void createFile(const Size initialSize);
   void openFile();

   virtual Size resize(Size newSize);
   int _fileNumber;
   
private:
   Size resize(Size newSize, int fileNumber);
   
private:
   FILE* _file = NULL;
   Size _size = -1;
   bool _isNew;
   Size getFileSize();
};

typedef File::Size Size;
