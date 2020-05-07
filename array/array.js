Array.prototype.toString = objectToString;
Array.prototype.toShorthand = arrayToShorthand;
Array.fromStorage = arrayFromStorage;
Array.prototype.save = saveObject;
Array.prototype.encode = encodeArray;
Array.decode = decodeArray;

function arrayToShorthand(
   shorthand)
{
      
   var array = this.encode(shorthand);
   
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
                  getPointer(element, shorthand);
               hasCustom = true;
            }
         }
      );
   
   if (!hasCustom)
      custom = undefined;
      
   var json = {
      "=": this["="]
              .toShorthand(shorthand),
      "[]": array,
      "{}": custom
   }
   
   return json;

 
}

function encodeArray(shorthand) {
   // create an array of pointers
   var array = new Array(this.length);
   
   for (var i = 0; i < this.length; ++i) {
      var element = this[i];
      array[i] =
         getPointer(element, shorthand);
   }
   
   return array;
}

function getPointer(element, shorthand) {
   
   var value;
   if ((element instanceof Object &&
       !(element instanceof Id)) ||
       Array.isArray(element))
   {
      // Get a pointer to element
      var pointer = new Pointer(element);
      value =
         pointer.toShorthand(shorthand);
   }
   else
      // Set value to unchanged
      value = element;
         
   return value;
}

function arrayFromStorage(input) {
   
   var id = new Id(input["="]);
   var data = input["[]"];
   var custom = input["{}"];
   
   if (Memory.map.has(id.key))
      return Memory.map.get(id.key);
      
   var Type = id.Type;
   
   var array = Type.decode(data, id);
   
   Object.assign(array, custom);
  
   array["="] = id;
 
   return array;
}

function decodeArray(data, id) {

   var array;
   if (id.Type.from instanceof Function)
      array = id.Type.from(data);
   else
      array = new id.Type(...data);
      
   Memory.map.set(id.key, array);
   
   array.forEach(
      function(element, index) {
         if (Pointer.isPointer(element))
         {
            var pointer =
               new Pointer(element);
            element = pointer.fetch();
            array[index] = element;
         }
      }
   );
   

   
   return array;
}


