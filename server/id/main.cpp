#include <stdio.h>
#include <iostream>
#include <vector>
#include "id.h"

using namespace std;
using namespace bee::fish::server;

int main(int argc, const char* argv[]) {

   fork();

   vector<Id> array(100);
   for (const Id& id : array)
   {
      cout << id << endl;
   }
   
}