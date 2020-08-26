#include <stdio.h> 
#include <boost/endian/conversion.hpp>
  
/* function to show bytes in memory, from location start to start+n*/

void show_mem_rep(char *start, int n)  
{ 

    int i; 

    for (i = 0; i < n; i++) 

         printf(" %.2x", start[i]); 

    printf("\n"); 
} 

  
/*Main function to call above function for 0x01234567*/

int main() 
{ 

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