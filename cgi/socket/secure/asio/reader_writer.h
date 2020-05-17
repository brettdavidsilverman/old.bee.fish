#ifndef READER_WRITER_H
#define READER_WRITER_H

class reader_writer {
public:
  // typedef void (*callback)(std::string data);
   
   reader_writer() {
   }
   
   virtual char
   read() = 0;
   
   virtual void
   write(char c) = 0;
   
   virtual void
   write(const std::string& data)
   {
      for (char c : data)
         write(c);
   }

};

#endif