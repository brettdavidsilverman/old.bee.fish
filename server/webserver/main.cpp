#include <iostream>
#include "httpd.h"

int main(int c, char** v)
{
    using namespace std;

    serve_forever("8080");

    return -1;
}

