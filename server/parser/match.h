#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "../id/id.h"

using namespace std;

inline ostream& operator <<
(ostream& out, optional<bool> ok)
{
   if (ok == true)
      out << "1";
   else if (ok == false)
      out << "0";
   else
      out << "?";
         
   return out;
}

using namespace bee::fish::server;

namespace bee::fish::parser {
			
			class Match {
			protected:
			
			   string _value = "";
			   wstring _wvalue = L"";
       size_t _lastMatched = 0;
       size_t _position = 0;
			   optional<bool> _success = nullopt;
			   
			public:
			   static const int EndOfFile = -1;
			   bool _capture = true;
			   
			   vector<Match*> _inputs;
			   
			   template<typename ...T>
			   Match(T*... inputs) :
			      _inputs{inputs...}
			      
			   {
			
			   }
			   
			   virtual ~Match() {
			      
			      for (auto
			             it = _inputs.cbegin();
			             it != _inputs.cend();
			           ++it)
			      {
			         Match* child = *it;
			         if (child)
			         {
			            delete child;
			         }
			      }
			      
			   }
			   
			   virtual bool match
			   (int character, optional<bool>& success)
			   {
			      
			      if ( _capture &&
			           character != Match::EndOfFile 
			         )
			      {
			         _value += (char)character;
			         _wvalue += (char)character;
			      }
			      
			      return true;
			   }
			
			   virtual bool read(
			      istream& in,
			      bool last = true
			   )
			   {
			      
			      _success = nullopt;
			      
			      cerr << endl;
			      
			      while (!in.eof())
			      {
			         int character = in.get();

             if (character == Match::EndOfFile)
                break;
#ifdef DEBUG
             Match::write(cerr, character);
#endif
             match(character, _success);
			         
			         if (_success != nullopt)
			            break;
			            
			      }

			      if ( _success == nullopt &&
			           last &&
			           in.eof()
			         )
			      {
#ifdef DEBUG
             Match::write(cerr, Match::EndOfFile);
#endif
			         match(Match::EndOfFile, _success);
			         
			         return _success == true;
			      }
			     
			      return (_success != false);
			   }
			   
			   virtual bool read(const string& str, bool last = true)
			   {
			      
			      istringstream in(str);
			      
			      return read(in, last);
			      
			   }
			   
			   virtual optional<bool> success()
			   {
			      return _success;
			   }
			   
			   virtual void onsuccess()
			   {
			      _success = true;
			   }
			   
			   virtual void onfail()
			   {
			      _success = false;
			   }
			 
			   friend ostream& operator <<
			   (ostream& out, Match& match) 
			   {
			
			      match.write(out);
			   
			      return out;
			
			   }
			   
			   virtual void write(ostream& out) 
			   {
			      
			      out << "{" 
			          << name() 
			          << ":{ok:\""
			          << _success
			          << "\"";
			 
			      for (auto it = _inputs.cbegin();
			                it != _inputs.cend();
			                ++it
			          )
			      {
			         out << ',';
			         Match* input = *it;
			         if (input)
			            out << *input;
			         else
			            out << "null";
			      }
			      
			      out << '}'
			          << '}';
			   }
			   
			   virtual string name() {
			      return "Match";
			   }
			   
			   virtual Match& operator[]
			   (size_t index)
			   {
			      return *(_inputs[index]);
			   }
			   
			   virtual string& value()
			   {
			      return _value;
			   }
			   
			   virtual wstring& wvalue()
			   {
			      return _wvalue;
			   }
			   
			   virtual size_t lastMatched()
			   {
			      return _lastMatched;
			   }
			   
			   virtual bool isOptional()
			   {
			      return false;
			   }
			   
			public:
			
			   
			   
			   static void write(ostream& out, int character)
			   {
			      switch (character) {
			      case '\"':
			         out << "\\\"";
			         break;
			      case '\\':
			         out << "\\\\";
			         break;
			      case '\b':
			         out << "\\b";
			         break;
			      case '\f':
			         out << "\\f";
			         break;
			      case '\r':
			         out << "\\r";
			         break;
			      case '\n':
			         out << "\\n";
			         break;
			      case '\t':
			         out << "\\t";
			         break;
			      case Match::EndOfFile:
			         out << "-1";
			         break;
			      default:
			         out << (char)character;
			      }
			   }
			
			   
			};
			
			
			
}
			
#endif
			
			
