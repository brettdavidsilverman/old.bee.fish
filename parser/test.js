const endOfFile = {};

class Match {
   #success = undefined;
   
   match(character) {
      throw new Error("Not implemented");
   }
   
   read(string) {
     
      var matched;
      
      for (var i = 0;
           i < string.length;
           ++i)
      {
         var character = string[i];
         
         var matched =
            this.match(character);
         
         if (matched)
            document.write("{" + escape(character) + "}");
            
         if (this.success || this.failed)
            return matched;
     
      }
      
      matched = this.match(endOfFile);
      
      return matched;
      
   }
   
   get success() {
      return this.#success;
   }
   
   set success(value) {
      if (value != this.#success)
      {
         this.#success = value;
         if (this.#success)
            this.onsuccess();
      }
   }
   
   onsuccess() {
   }
   
}


            
class Character extends Match {
   #character;
   
   constructor(character) {
      super();
      this.#character = character;
   }
   
   match(character) {
      var matched =
         this.#character === character;
         
      if (matched) {
         this.success = true;
         this.value = character;
      }
      else
         this.failed = true;
         
      return matched;
   }
}
            
class Range extends Match {
   #minimum;
   #maximum;
   constructor(minimum, maximum) {
      super();
      this.#minimum = minimum;
      this.#maximum = maximum;
   }
   
   match(character) {
   
      var matched =
         (this.#minimum <= character) &&
         (this.#maximum >= character);
         
      if (matched) {
         this.value = character;
         this.success = true;
      }
      else
         this.failed = true;
      
      return matched;
   }
}
            
class Word extends Match {
   #index = 0;
   #word;
   value = "";
   constructor(word) {
      super(word);
      this.#word = word;
   }
   
   match(character) {
      var matched =
         this.#word[this.#index] ===
         character;
         
      if (matched)
      {
         this.value += character;
         ++this.#index;
         if (this.#index === this.#word.length)
            this.success = true;
      }
      else
         this.failed = true;
         
      return matched;
   }
   
   
}

            
class Or extends Match {
   #array;
   constructor(...array) {
      super();
      this.#array = array;
   }
   
   match(character) {
      var matched = false;
 
      for (var i = 0;
           i < this.#array.length;
           i++)
      {
         var item = this.#array[i];
         if (!item.failed) {
            if (item.match(character)) {
               matched = true;
               if (item.success) {
                  this.value = item.value;
                  this.index = i;
                  this.success = true;
                  return true;
               }
            }
         }
      }
      
      if (!matched)
         this.failed = true;
         
      return matched;
   }
}
            
class And extends Match {
   #index = 0;
   #array;
   #value = [];
   constructor(...array) {
      super();
      this.#array = array;
      if (this.#array.length === 0) {
         this.failed = true;
      }
   }
   
   match(character) {
   
      var item;
      var matched;
      
      do {
         item = this.#array[this.#index];
         matched = item.match(character);
    
         if (item.success) {
            this.#value.push(item.value);
            if (++this.#index ===
                this.#array.length) {
               this.value = this.#value;
               this.success = true;
               return matched;
            }
         }
         else if (item.failed) {
            this.failed = true;
         }
         
      } while(item.success && !matched);
      
      if (!matched) {
         this.failed = true;
      }
      
      return matched;
   }
   
   get items() {
      return this.#array;
   }
}
            
class Optional extends Match {
   #optional;
   constructor(optional) {
      super();
      this.#optional = optional;
      this.success = true;
   }
   
   match(character) {
      var matched =
         this.#optional.match(character);
      
      if (this.#optional.success) {
         this.value =
            this.#optional.value;
      }
         
      return matched;
   }
}
            
class Not extends Match {
   #match;
   constructor(match) {
      super();
      this.#match = match;
      this.value = "";
   }
   
   match(character) {

      var matched =
         this.#match.match(character);
      
      if (!matched) {
         this.value += character;
         this.success = true;
      }
      else if (this.#match.success) {
         this.failed = true;
      }
      
      return !matched;
      
   }
   
}
            
class Repeat extends Match {
   #Match;
   #match;
   #values = [];
   value = undefined;
  
   constructor(Match, Join) {
      super();
      this.#Match = Match;
      this.#match = new this.#Match();
      
   }
   
   match(character) {
   
      var matched =
         this.#match.match(character);
         
      if (this.#match.success) {
 
            this.#values.push(
               this.#match.value
            );
            
         this.#match =
            new this.#Match();
           
      }
      
      
      
      if (!matched ||
          character === endOfFile) {
            
        this.value = this.#values;
        this.success = true;

      }
      
      return matched;
      
   }
   
}


            
class Capture extends Match {

   #object;
   #keys;
   #and;
   
   constructor(object) {
      super();
      this.#object = object;
      this.#keys = Object.keys(object);
      this.#and = new And(
         ...Object.values(this.#object)
      );
   }
   
   match(character) {
      var matched =
         this.#and.match(character);
        
      if (this.#and.success) {
         var i = 0;
         var value = this;
         this.#and.value.forEach(
            (item) => {
               var key = this.#keys[i++];
               value[key] = item;
            }
         );
         this.value = value;
         this.success = true;
      }
      else if (!matched) {
         this.failed = true;
      }
      
      return matched;

   }
   
}
            
class WhitespaceCharacter extends Or {
   constructor() {
      super(
         new Character(" "),
         new Character("\t")
      );
   }
}

class Whitespace extends Repeat {

   constructor() {
      super(WhitespaceCharacter);
   }
   
   
}
            
class Colon extends And {
   constructor() {
       super(
          new Optional(new Whitespace()),
          new Character(":"),
          new Optional(new Whitespace())
       );
   }
   
}
            
class NewLine extends Match {
   static standard = "\r\n";
   #firstChar = null;
   
   match(character) {
      if (this.#firstChar === null)
      {
         if (character === "\r" ||
             character === "\n")
         {
            this.#firstChar = character;
            return true;
         }
      }
      else if (this.#firstChar === "\r")
      {
         if (character === "\n" ||
             character === endOfFile)
         {
            this.success = true;
            return true;
         }
      }
      else if (this.#firstChar === "\n")
      {
         this.success = true;
         return false;
      }
      else
      {
         this.failed = true;
         return false;
      }
   }
   
   onsuccess() {
      super.onsuccess();
      this.value = NewLine.standard;
   }
}
             
class FirstIdentifierCharacter
   extends Or
{
   constructor() {
      super(
         new Range("a", "z"),
         new Range("A", "Z"),
         new Character("_")
      )
   }
}

class SubsequentIdentifierCharacter
   extends Or
{
   constructor() {
      super(
         new Range("0", "9"),
         new Range("a", "z"),
         new Range("A", "Z"),
         new Character("_")
      )
   }
}

class Identifier extends And
{
   #value = "";
   constructor() {
      super(
         new FirstIdentifierCharacter(),
         new Repeat(
            SubsequentIdentifierCharacter
         )
      )
   }
   
   match(character) {
      var matched =
         super.match(character);
      if (matched)
         this.#value += character;
      if (this.success)
         this.value = this.#value;
      return matched;
   }
   
}
             
class PathCharacter extends Or {
   constructor() {
      super(
         new Character("/"),
         new Character("."),
         new Character("-"),
         new Character("_"),
         new Range("a", "z"),
         new Range("A", "Z")
      )
   }
}

class Path extends Repeat {
   constructor() {
      super(
         PathCharacter
      )
   }
   
   onsuccess() {
      this.value = this.value.join("");
   }

}
            
class FirstLine extends Capture {

   constructor() {
      super( {
         verb: new Identifier(),
         whitespace1: new Whitespace(),
         path: new Path(),
         whitespace2: new Whitespace(),
         version:
            new Word("HTTP/1.1"),
         newLine:
            new NewLine()
     } );
   }
}
            
class HeaderCharacter extends Not {
   constructor() {
      super(
         new Or(
            new NewLine(),
            new Character(":")
         )
      );
   }
}

class HeaderName extends Repeat {
   constructor() {
      super(
         HeaderCharacter
      );
   }
}

class HeaderValue extends Repeat {
   constructor() {
      super(
         HeaderCharacter
      );
   }
}
            
class HeaderLine extends Capture {
   constructor() {
      super(
         {
            name: new HeaderName(),
            colon: new Colon(),
            _value: new HeaderValue(),
            newLine: new NewLine()
         }
      );
   }
   
   write() {
      document.write(
         this.name.join("")
      );
      document.write(":\t");
      document.writeln(
         this._value.join("")
      );
   }
}

class Headers extends Repeat {
   constructor() {
      super(HeaderLine)
   }
}

            
class Request extends Capture {
   constructor() {
      super(
         {
            firstLine: new FirstLine(),
            headers: new Headers()
         }
      );
   }
}
            