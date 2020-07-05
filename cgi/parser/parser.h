#ifndef PARSER_H
#define PARSER_H
#include <string>
#include "match.h"
#include "character.h"
#include "range.h"
#include "word.h"

using namespace std;

class Error {
private:
   string _message;
   
public:
   Error(const string message) {
      _message = message;
   }
};


#endif