#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <iostream>

using namespace std;

namespace Bee::Fish::Parser {

class Match {

protected:
   virtual bool match(int character) = 0;
   bool _success = false;
   bool _failed = false;
   string _value;
   
public:
   bool read(string s, bool endOfFile = false) {
   
      bool matched;
		          
      for (const char character : s)
      {
							   matched = match(character);
									           
									 if (matched)
										   cout << "{"
										        << character
										        << "}";
										               
									 if (_success || _failed)
									    return matched;
      }
				  
      if (endOfFile)
         matched = match(-1);
					      
      return matched;
					            
   }

   bool getSuccess() const {
      return _success;
			}
			
			bool getFailed() const {
			   return _failed;
			}
		    
		 void setSuccess(bool value) {
      if (value != _success) {
						    _success = value;
								  if (_success)
										   onsuccess();
							}
			}
			
			void setFailed(bool value) {
      if (value != _failed) {
						    _failed = value;
								  if (_failed)
										   onfailed();
							}
			}
		       
		 virtual void onsuccess() {
	  }
	  
	  virtual void onfailed() {
	  }
	  
	  const string getValue() const {
	     return _value;
	  }
};

}

ostream& operator << 
(
   ostream& out,
   const Bee::Fish::Parser::Match& match
);


#endif


