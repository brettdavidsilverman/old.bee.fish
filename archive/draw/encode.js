function encode(object) {

   // traverse the tree, transforming
   // functions and number arrays into
   // strings
   var root = new Node(
      {
         parent: null,
         leaf: object
      }
   );
   
   var node = root;
   
   while (node) {
   
      if ( typeof node.leaf == 
           "function"
         )
      {
         node.setValue(
            encodeFunction(node.leaf)
         );
      }
      else if (isNumberArray(
                  node.leaf
               )
            )
      {
         node.setValue(
            encodeNumberArray(
               node.leaf
            )
         )
      }
      
      node = node.next();
   }
   
   
   var string = JSON.stringify(object);
   
   var base64 = btoa(string);
   
   return base64;
}

function decode(base64) {

   var string = atob(base64);
   
   var object = JSON.parse(string);
   
   var root = new Node(
      {
         parent: null,
         leaf: object
      }
   );
   
   var node = root;
   
   while (node) {
   
      if ( isEncodedFunction(
              node.leaf
           )
         )
      {
         node.setValue(
            decodeFunction(node.leaf)
         );
      }
      else if ( isEncodedNumberArray(
                   node.leaf
                )
              )
      {
         node.setValue(
            decodeNumberArray(node.leaf)
         );
      }
      
      node = node.next();
   }
   
   return object;
}

function isEncodedFunction(object) {

   if (typeof(object) == "string") {
      var string = object;
      if (string.startsWith("[code](")
           &&
          string.endsWith(")")
         )
         return true;
   }
   
   return false;
}

function encodeFunction(func) {

   return "[code]" + 
          "(" + 
              String(func) + 
          ")";

}

function decodeFunction(string) {

   return
      alert(eval(
         string.substr(
            "[code]".length
         )
      ));

}

function isNumberArray(object) {
  
   if (!Array.isArray(object))
      return false;
      
   var array = object;
   var isAllNumbers = true;
  
   array.map(
      function(item) {
         if (isNaN(item))
            isAllNumbers = false;
      }
   );
      
   return isAllNumbers;
   
}

function isEncodedNumberArray(object) {
   return typeof(object) == "string" &&
          object.startsWith("_[") &&
          object.endsWith("]");
}


function encodeNumberArray(
      numberArray
   )
{
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
   
   // convert sfring to base 64
   var base64 = btoa(string);
   
   // wrap base 64 with prefix
   // and suffix
   return "_[" + base64 + "]";
}

function decodeNumberArray(string) {
 
   // get rid of prefix _[ and suffix ]
   string = string.substr(2, string.length - 3);
   
   // decode base64
   string = atob(string);
   
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
      binary += String.fromCharCode(
         bin
      );
         
   }
      
   return binary;
}


function binaryToChar(binary) {
   var string = "";
   for ( var i = 0;
         i < binary.length;
         i++)
   {
      var c = binary
         .charCodeAt(i);
            
      var cLow  = c & 0x00FF;
      var cHigh = c >> 8;
         
      string +=
         String.fromCharCode(cHigh)
          +
         String.fromCharCode(cLow);
   }
      
   return string;
}


