#include <stdio.h> 
#include <boost/endian/conversion.hpp>
#include <iostream>
#include <sstream>

/* function to show bytes in memory, from location start to start+n*/

void show_mem_rep(char *start, int n)  
{ 

    int i; 

    for (i = 0; i < n; i++) 

         printf(" %.2x", start[i]); 

    printf("\n"); 
} 

  
/*Main function to call above function for 0x01234567*/
using namespace std;

int main() 
{ 

   if (std::numeric_limits<double>::is_iec559)
      printf("IEEE-754\n");
      
   string line;
   while (!cin.eof()) {
      getline(cin, line);
      if (line.length() == 0)
         break;
      stringstream in(line);
      double dbl;
      in >> dbl;
      cout << dbl << endl;
   }
   
   int i = 0x01234567; 
   printf("Number:\t0x01234567\n");
   
   printf("Native:\t");
   show_mem_rep((char *)&i, sizeof(i)); 
   
   printf("Big:\t");
   i = 0x01234567;
   i = boost::endian::native_to_big(i);
   show_mem_rep((char *)&i, sizeof(i));
   
   printf("Little:\t");
   i = 0x01234567;
   i = boost::endian::native_to_little(i);
   show_mem_rep((char *)&i, sizeof(i));
   
   getchar(); 

   return 0; 
} 