function numberArrayToString(numberArray) {
   var length = numberArray.length;
   var buffer = new ArrayBuffer(length * 64 / 8);
   var view = new DataView(buffer);

   for (var i = 0; i < length; i++) {
      view.setFloat64(i * 8, numberArray[i]);
   }
   
   var string = "";
   
   for (var i = 0; i < length * 8; i++) {
      var character = view.getUint8(i);
      string += String.fromCharCode(character);
   }
   
   return string;
}

function stringToNumberArray(string) {
 
   // create a buffer to hold the numbers
   var buffer = new ArrayBuffer(string.length);
   
   // create a view to access the buffer
   var view = new DataView(buffer);
   
   // write to the view a byte at a time
   for (var i = 0; i < string.length; i++) {
      view.setUint8(
         i,
         string.charCodeAt(i)
      );
   }
   
   // read from the view 8 bytes, or
   // float64 at a time
   var array = new Array(string.length/8);
   for (var i = 0; i < array.length; i++) {
      var number =  view.getFloat64(
         i * 8
      );
      array[i] = number;
   }
   
   return array;
}

function encode(array) {
   
   var float64Array = Float64Array.from(array);

   var uint8Array = new Uint8Array(float64Array.buffer);
   var string = "";
   uint8Array.forEach(
      function (uint8) {
         string += String.fromCharCode(uint8);
      }
   );
   var str = btoa(string);

   return str;
}

function decode(string) {

   var str = atob(string);

   var uint8Array = new Uint8Array(str.length);

   for (var i = 0; i < str.length; i++) {
      uint8Array[i] = str.charCodeAt(i);
   }
   
   var float64Array = new Float64Array(uint8Array.buffer);
   
   var array = new Array(float64Array.length);
   
   for (var i in float64Array) {
      array[i] = float64Array[i];
   }
   
   return array;
}
