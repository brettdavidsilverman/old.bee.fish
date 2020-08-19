#pragma once
#ifndef BEE_FISH_PARSER_JSON__OBJECT_H
#define BEE_FISH_PARSER_JSON__OBJECT_H

#include "../parser.h"
#include "blank-space.h"
#include "json.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json {
   
   class Object : 
      public And,
      public map<wstring, JSON*>
   {
   public:
      inline static vector<wstring> path;
      
      Object() : And(
         new OpenBrace(),
         new Optional(
            new BlankSpace()
         ),
         new Optional(
            new And(
               new Field(this),
               new Optional(
                  new Records(this)
               )
            )
         ),
         new Optional(
            new BlankSpace()
         ),
         new CloseBrace()
      )
      {
      }
      /*
      virtual void onsuccess()
      {
         delete this;
      }
      */
      
      class OpenBrace : public Character
      {
      public:
         OpenBrace() : Character('{')
         {
         }
         
         virtual void onsuccess()
         {
            Object::path.push_back(L"");
            Character::onsuccess();
         }
      };
      
      class CloseBrace : public Character
      {
      public:
         CloseBrace() : Character('}')
         {
         }
         
         virtual void onsuccess()
         {
            Object::path.pop_back();
            Character::onsuccess();
         }
      };
      
      class Identifier : public And
      {
      protected:
         wstring _wvalue;
         
      public:
         Identifier() : And(
            new Or(
               new And(
                  new Character('_'),
                  new Or(
                     new Range('0', '9'),
                     new Range('A', 'Z'),
                     new Range('a', 'z')
                  )
               ),
               new Or(
                  new Range('A', 'Z'),
                  new Range('a', 'z')
               )
            ),
            new Optional(
               new Repeat<IdentifierCharacter>()
            )
         )
         {
         }
            
         virtual bool match
         (
            int character, 
            optional<bool>& success
         )
         {
            bool matched =
               And::match(
                  character, success
               );
               
            if ( _capture &&
                 matched &&
                 character != Match::endOfFile)
            {
               _wvalue += character;
            }
               
            return matched;
         }
         
         class IdentifierCharacter :
             public Or
         {
         public:
            IdentifierCharacter() : Or(
               new Character('_'),
               new Range('0', '9'),
               new Range('A', 'Z'),
               new Range('a', 'z')
            )
            {
            }
               
         };
            
         virtual wstring& wvalue()
         {
            return _wvalue;
         }
      };
      
      class Label : public Or
      {
      public:
         Label() : Or(
            new String(),
            new Identifier()
         )
         {
            _inputs[0]->_capture = true;
            _inputs[1]->_capture = true;
         }
               
         virtual string& value()
         {
            return item().value();
         }
            
         virtual wstring& wvalue()
         {
            if (_index == 0)
               return (
                  (String&)(item())
               ).wvalue();
            else
               return ( 
                  (Identifier&)(item())
               ).wvalue();
         }
         
         
         virtual void onsuccess()
         {
            vector<wstring>& _path = path;
            _path[_path.size() - 1] =
               wvalue();
            Or::onsuccess();
           
         }
      };

      class Field : public And
      {
      private:
         Object* _object;
         
      public:
         Field(Object* object) : And(
            new Label(),
            new Optional(
               new BlankSpace()
            ),
            new Character(':'),
            new Optional(
               new BlankSpace()
            ),
            new LazyLoad<JSON>()
         )
         {
            _object = object;
            _inputs[0]->_capture = true;
            _inputs[4]->_capture = true;
         }
         
         Object& object()
         {
            return *_object;
         }
         
         Label& label()
         {
            Field& field = *this;
            Label& label =
               (Label&)(field[0]);
            return label;
         }
            
         JSON& item()
         {
            Match& field = *this;
            LazyLoad<JSON>& lazyLoad =
               (LazyLoad<JSON>&)(field[4]);
            JSON& value = lazyLoad.item();
            return value;
         }
            
         virtual void onsuccess()
         {
            for (auto it = path.begin();
                      it != path.end();
                    ++it)
            {
               wcout << *it << ".";
            }
            
            std::wstring wvalue =
               (*this)[4].wvalue();
            
            wcout << wvalue << endl;
            
            And::onsuccess();
         }
      };
      
      class Record : public And
      {
      public:
          Record(Object* object = NULL) : And(
            new Optional(
               new BlankSpace()
            ),
            new Character(','),
            new Optional(
               new BlankSpace()
            ),
            new Field(object)
              
         )
         {
         }
            
         Field& field() {
            return (Field&)((*this)[3]);
         }
      };
         
      class Records : public 
         Repeat<Record>
      {
         Object* _object;
            
      public:
         Records(Object* object) : Repeat()
         {
            _object = object;
         }
            
         Record* createItem()
         {
            return new Record(_object);
         }
            
      };


      virtual string name()
      {
         return "Object";
      }
         
      virtual bool contains(
         const wstring& key
      )
      {
         return (count(key) > 0);
      }
      
      virtual JSON& operator[]
      (const wstring& key)
      {
         JSON* json = at(key);
         return *json;
      }
         
    
   };
}

#endif