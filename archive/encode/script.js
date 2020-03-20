function encode(value) {

   var binary = encodeValue(value);
   //var binary = JSON.stringify(value);
    
  // var string = btoa(binary);
   
   return binary;

   function encodeValue(value) {

      var string = "";
      
     // string = JSON.stringify(object);
      
     // return string;
      var type = typeof value;
      
      if (Array.isArray(value)) {
        
         if (isNumberArray(value))
            type = "numberArray";
         else
            type = "array";
         
      }
      
      if (value === null)
         type = "null";
      else if (value === undefined)
         type = "undefined";
         
      switch (type) {
      case "undefined":
         return "";
      case "null":
         return "!";
      case "numberArray":
         return "_" +
            encodeNumberArray(value);
      case "array":
         return "[" +
            encodeArray(value);
      case "string":
         return "\"" + 
            encodeString(value);
      case "number":
         return "0" +
            encodeNumber(value);
      case "object":
         return "{" +
            encodeObject(value);
      case "function":
         return "f" +
            encodeFunction(value);
      default:
         throw "Unexpected type " + 
            type;
      }
   }
   
   
   
   function isNumberArray(array) {
      var isAllNumbers = true;
      array.forEach(
         function(item) {
            if (isNaN(item))
               isAllNumbers = false;
         }
          
      );
      
      return isAllNumbers;
   }
   
   function encodeFunction(f) {

      return encodeString(
         String(f)
      );

   }
   
   function encodeString(string) {
   
      string =
         encodeNumber(string.length * 2) +
         binaryToChar(string);
      
      return string;
   }
   
   function binaryToChar(binary) {

      var string = "";
      for ( var i = 0;
            i < binary.length;
            i++)
      {
         var c = binary
            .charCodeAt(i);
         
         var cHigh = c >> 8;
         var cLow  = c & 0x00FF;
         
         string +=
            String
            .fromCharCode(cHigh)
            +
            String
            .fromCharCode(cLow);
      }

      return string;
   }
   
   function encodeArray(array) {
      var string = encodeNumber(array.length);
      var items =
         array.map(
            function(item) {
               return encodeValue(item);
            }
         );
      string += items.join("");
      
      return string;
   }
   
   function encodeNumberArray(
      numberArray
   )
   {
      var length = numberArray.length;
      var buffer = new ArrayBuffer(
         length * 64 / 8
      );
      var view = new DataView(buffer);

      for (var i = 0; i < length; i++) {
         view.setFloat64(i * 8, numberArray[i]);
      }
   
      var string = encodeNumber(length);
   
      for (var i = 0;
           i < length * 8;
           i++) 
      {
         var character =
            view.getUint8(i);
         string +=
            String
            .fromCharCode(character);
      }
   
      return string;
   }

   function encodeNumber(number) {
      // creating typed array to
      // contain single 64-bit IEEE754 
      var float64Array =
         new Float64Array(1);
      
      // transferring JSNumber
      // for untyped array to 
      // first element of typed one 
      float64Array[0] = number;
      
      // creating raw view on
      // content in typed array
      var dataView = new DataView(
         float64Array.buffer
      );

      // accessing bytes 0 to 3
      // of typed array 
      var word1 =
         dataView.getUint32(0, true);
    
      // accessing bytes 4 to 7
      // of typed array
      var word2 = 
         dataView.getUint32(4, true); 
      
      var string =
         encodeUint32(word1) +
         encodeUint32(word2);
      
      return string;
         
      function encodeUint32(number) {
         var string =
            String.fromCharCode(
               (
                  number & 0xFF000000
               ) >> 24
            ) +
            String.fromCharCode(
               (
                  number & 0x00FF0000
               ) >> 16
            ) +
            String.fromCharCode(
               (
                  number & 0x0000FF00
               ) >> 8
            ) +
            String.fromCharCode(
               (
                  number & 0x000000FF
               )
            );
            
         return string;
      }
   }
   
   function encodeObject(object) {

      var keys = Object.keys(object);
      var string = "";
      keys.forEach(
         function(key) {
            var value = object[key];
            if (key == null ||
                key == undefined)
               return "";
               
            string +=
               encodeString(key) +
               encodeValue(value);
         }
      );
      
      return string + "}";

   }
   
}

function decode(string) {

   var characters = (string);
  
   //var object = JSON.parse(binary);
   
   //return object;
   
   return decodeValue(
      characters,
      {
         index: 0
      }
   );
   
   function decodeValue(binary, i) {
         
      var character = binary[i.index++];

      switch (character) {
      case "!":
         return null;
      case "0":
         return decodeNumber(
            binary, i
         );
      case "\"":
         return decodeString(
            binary, i
         );
      case "[":
         return decodeArray(
            binary, i
         );
      case "_":
         return decodeNumberArray(
            binary, i
         );
      case "[":
         return decodeArray(
            binary, i
         );
      case "{":
         return decodeObject(
            binary, i
         );
      case "f":
         return decodeFunction(
            binary, i
         );
      default:
         var error =
            "Unexpected character '" +
            character + "' at position " +
            String(i.index);
         throw error;
         
      }
         
      return value;
   }
   

   function decodeObject(binary, i) {
      var object = {};
      
      var character;
         
      for ( ; ; ) {
      
         var key = decodeString(
            binary, i
         );
         
         var value =
            decodeValue(binary, i);
         
         object[key] = value;
         
         var character =
            binary[i.index];
         
         if (character == "}") {
            i.index++;
            break;
         }
               
      }
      
      return object;
   }
      
      
   function decodeNumber(binary, i) {
      // creating typed array to
      // contain single
      // 64-bit IEEE754
      var float64Array =
         new ArrayBuffer(8);
         
      // creating raw view on
      // content in typed array
      var dataView =
        new DataView(
           float64Array
        ); 
         
      var word1 =
         getUint32(binary, i);
            
      var word2 =
         getUint32(binary, i);

      dataView.setUint32(
         0,
         word1,
         true
      );
         
      dataView.setUint32(
         4, 
         word2,
         true
      );
         
      var number =
         dataView
         .getFloat64(0, true);
         
      return number;
         
      function getUint32(binary, i) {
         var number = (
            (
               binary
               .charCodeAt(i.index)
                << 24
            ) |
            (
               binary
               .charCodeAt(i.index+1)
               << 16
            ) |
            (
               binary
               .charCodeAt(i.index+2)
                << 8
            ) |
            (
               binary
               .charCodeAt(i.index+3)
            )
         );
         i.index += 4;
         return number;
      }

   }
      
   function decodeString(binary, i) {

      var length = decodeNumber(
         binary, i 
      );
         
      var string = binary.substr(
         i.index, length
      );
      i.index += length;
      string = charToBinary(string);
       
      return string;
   }
   
   function charToBinary(string) {
 
      var binary = "";
      for ( var i = 0;
            i < string.length;
            i += 2 )
      {
         var cHigh = string
            .charCodeAt(i);
         var cLow = string
            .charCodeAt(i + 1);
         var bin = (cHigh << 8) +
            cLow;
         binary +=
            String.fromCharCode(
               bin
            );
         
      }
      
      return binary;
   }
      
   function decodeArray(binary, i) {
      var length = decodeNumber(
         binary, i
      );
      var array = new Array(length);
      for (var index = 0; 
           index < length; 
           index++)
      {
         array[index] = decodeValue(
            binary, i
         );
      }
         
      return array;
   }
      
   function decodeNumberArray(
      binary,
      i
   ) {
 
      // get the length
      var length = decodeNumber(
         binary, i
      );

      // create a buffer to
      // hold the numbers
      var buffer =
         new ArrayBuffer(
            length * 8
         );
   
      // create a view to access
      // the buffer
      var view = new
         DataView(buffer);
   
      // write to the view
      // a byte at a time
      for (var index = 0;
           index < length * 8; 
           index++)
      {
         var character =
            binary.
            charCodeAt(
               i.index + index
            );
               
         view.setUint8(
            index,
            character
         );
      }
         
      i.index += length * 8;
         
      // read from the view
      // 8 bytes, or float64
      // at a time
      var array = new Array(
         length
      );
         
      for (var index = 0;
           index < array.length; 
           index++)
      {
         var number = 
            view.getFloat64(
               index * 8
            );
         array[index] = number;
      }
   
      return array;
   }

   function decodeFunction(binary, i) {
      var string = decodeString(
         binary, i
      );
      var f = eval("(" + string + ")");
      return f;
   }
      
}
   
   
