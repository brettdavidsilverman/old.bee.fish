#include <iostream>

using namespace std;

int main(int argc, char* argv[], char* envp[]) {

//   cout << "HTTP/1.1 200 OK" << endl;
   
   cout << "Content-type: text/plain"
        << endl
        << endl;
   
   cout << "ARGS" << endl;
   
   for (int i = 0; i < argc; ++i) {
      char* arg = argv[i];
      cout << arg << endl;
   }
   
   cout << endl;
   
   cout << "QUERY_STRING" << endl;
   char* queryString = getenv("QUERY_STRING");
   if (queryString)
      cout << queryString << endl;
      
   cout << endl;
   
   cout << "ENVS" << endl;
   
   for (char **env = envp; *env != 0; env++) {
      cout << *env << endl;
   }
   
   cout << endl;
   
   cout << "CIN" << endl;
   char c;
   while (!cin.eof()) {
      cin >> c;
      cout << c;
   }
   
   return 0;
}


