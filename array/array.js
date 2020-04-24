Array.prototype.toString = objectToString;
Array.prototype.toJSON = arrayToJSON;
Array.fromJSON = arrayFromJSON;
Array.prototype.save = saveObject;

function arrayToJSON(property)
{
   if (property === "[]")
      return this;
      
   if (Shorthand.is(Shorthand.POINTERS) &&
       !Shorthand.is(Shorthand.ARRAY)) {
      var pointer = new Pointer(this);
      return pointer.toJSON();
   }
   
   // create an array of pointers
   var array = new Array(this.length);
   
   for (var i = 0; i < this.length; ++i) {
      var element = this[i];
      array[i] = getPointer(element);
   }
   
   // Add extra custom fields
   var custom = {};
   var object = this;
   var hasCustom = false;
   Object
      .keys(this)
      .slice(this.length)
      .forEach(
         function(field) {
            if (field != "=") {
               var element = object[field];
               custom[field] =
                  getPointer(element);
               hasCustom = true;
            }
         }
      );
   
   if (!hasCustom)
      custom = undefined;
      
   var json = {
      "=": this["="],
      "[]": array,
      "{}": custom
   }
   
   return json;

   function getPointer(element) {
   
      if (!Shorthand.is(Shorthand.POINTERS))
         // Return the element unchanged
         return element;
         
      var value;
      if ((element instanceof Object &&
          !(element instanceof Id)) ||
          Array.isArray(element))
         // Get a pointer to element
         value = new Pointer(element);
      else
         // Set value to unchanged
         value = element;
         
      return value;
   }
}

function arrayFromJSON(input, memory) {
   
   var id = new Id(input["="]);
   var data = input["[]"];
   var custom = input["{}"];
   
   var array;
   var typeFunction = id.typeFunction;
   if (typeFunction.from
       instanceof Function) {
      array = typeFunction.from(data);
   }
   else
      array = new typeFunction(...data);
      
 
   Object.assign(array, custom);
 
   Object.keys(array).forEach(
      function(i) {
         var element = array[i];
         if (Pointer.isPointer(element)) {
            var pointer = new Pointer(element);
            element = pointer.fetch(memory);
         }
         array[i] = element;
      }
   );
  
   array["="] = id;
 
   return array;
}
