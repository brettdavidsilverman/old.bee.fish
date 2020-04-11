Array.prototype.toString = arrayToString;
Array.fromJSON = arrayFromJSON;
Array.prototype.encode = mapToPointers;
Array.prototype.save = save;

function arrayToString(
   shorthand = Shorthand.HUMAN
   )
{
 
   var id = this["="];
   
   var custom = {}
   var object = this;
   var hasProperties = false;
   
   new Shorthand(Shorthand.POINTERS);
   
   // Map all custom properties
   // to the json object
   Object.keys(this).slice(this.length)
      .forEach(
         function(property) {
            var value = object[property];
            if (!(value instanceof Id)) {
               hasProperties = true;
               custom[property] = value;
            }
         }
      );
   
   Shorthand.pop();
   
   if (!hasProperties) {
      custom = undefined;
   }
   
   // Map all items in the array to
   // pointers.
   var array = this.encode();
   
   // Create the composite object
   var json = {
      "[]" : {
         "=": id,
         "*": array,
         bpe: this.BYTES_PER_ELEMENT,
         "{}": custom
      }
   };
  
   new Shorthand(shorthand);
   
   // Get the string value.
   var indent = "   ";
   if (shorthand === Shorthand.POINTERS)
      indent = "   ";
      
   var string = JSON.stringify(
      json,
      null,
      indent
   );
   
   Shorthand.pop();
   
   return string;
}

function mapToPointers() {
   return this.map(
      function(element) {
      
         if (element instanceof Object &&
             !(element instanceof Id) &&
             !(element instanceof Pointer))
             return new Pointer(element);
          
          return element;
      }
   );
}

function arrayFromJSON(input, memory) {

   var id = input["="];
   var typeFunction = id.typeFunction;

   var data = input["*"];
   var array;
   if (typeFunction.from
       instanceof Function) {
      array = typeFunction.from(data);
   }
   else
      array = new typeFunction(...data);
      
   array["="] = id;
   
   if ("{}" in input)
      Object.assign(array, input["{}"]);
 
   array.forEach(
      function(element, index) {
         if (element instanceof Object &&
             "->" in element) {
             var pointer = new Pointer(element);
             element = pointer.fetch(memory);
             array[index] = element;
         }
      }
   );
   
   return array;
}


