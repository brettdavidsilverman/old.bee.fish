Array.prototype.toString = arrayToString;
Array.fromJSON = arrayFromJSON;
Array.prototype.encode = mapToPointers;
//Array.prototype.save = save;

function arrayFromJSON(input, memory) {

   var array = Array.from(input.data);
   
   array["="] = input["="];
   
   if ("{}" in input)
      Object.assign(array, input["{}"]);
 
   array.forEach(
      function(element, index) {
         if (element instanceof Object &&
             "->" in element) {
             var pointer = new Pointer(element);
             element = pointer.fetch(memory);
             this[index] = element;
         }
      }
   );
   
   return array;
}

function arrayToString(
   shortHand = ShortHand.human
   )
{
   ShortHand.current = shortHand;
 
   var id = this["="];
   
   var json = {}
   var object = this;
   var hasProperties = false;
   
   // Map all custom properties
   // to the json object
   Object.keys(this).slice(this.length)
      .forEach(
         function(property) {
            var value = object[property];
            if (!(value instanceof Id)) {
               hasProperties = true;
               json[property] = value;
            }
         }
      );
      
   if (!hasProperties) {
      json = undefined;
   }
   
   // Map all items in the array to
   // pointers.
   var array = this.encode();
   
   // Create the composite object
   var value = {};
   
   // set the id
   value["="] = id;
   
   // Set thd encoded data
   value.data = array;
   
   // Add any extra fields
   if (json)
      value["{}"] = json;
      
   // This field is used by typed arrays
   value.bpe = this.BYTES_PER_ELEMENT;
   
   // Get the string value.
   var string = JSON.stringify(
      value,
      null,
      "   "
   );
   
   return string;
   
   
}

function mapToPointers() {
   return this.map(
      function(element) {
         if (element instanceof Object &&
             !(element instanceof Id) &&
             !(element instanceof Pointer))
             return new Pointer(element);
          else
             return element;
      }
   );
}
