defineTypedArray(Int8Array);
defineTypedArray(Uint8Array);
defineTypedArray(Uint8ClampedArray);
defineTypedArray(Int16Array);
defineTypedArray(Uint16Array);
defineTypedArray(Int32Array);
defineTypedArray(Uint32Array);
defineTypedArray(Float32Array);
defineTypedArray(Float64Array);
defineTypedArray(BigInt64Array);
defineTypedArray(BigUint64Array);

function defineTypedArray(Type) {
   
   Type.prototype.toString = objectToString;
   Type.prototype.toJSON = arrayToJSON;
   Type.fromJSON = arrayFromJSON;
   Type.prototype.save = saveArray;
   Type.prototype.encode = encodeTypedArray;
   Type.decode = decodeTypedArray;
   Object.defineProperty(
      Type.prototype,
      "=",
      {
         get: getId,
         set: setId,
         enumerable:   true,
         configurable: true
      }
   );
}

var getEndianIndex;

function encodeTypedArray() {

   if (Shorthand.is(Shorthand.HUMAN))
      return Array.from(this);
      
   var dataView = new DataView(this.buffer);
   var chars = "";
   
   for ( var i = 0;
         i < this.length *
             this.BYTES_PER_ELEMENT;
         ++i )
   {
      var x = getEndianIndex(
         i,
         this.BYTES_PER_ELEMENT
      );
      var c = dataView.getUint8(x);
      chars += String.fromCharCode(c);
   }
   
   return btoa(chars);
}

function decodeTypedArray(data, Type, memory) {
   var chars = atob(data);
   var buffer = new ArrayBuffer(chars.length);
   var dataView = new DataView(buffer);
   for ( var i = 0;
         i < chars.length;
         ++i )
   {
      var c = chars.charCodeAt(i);
      var x = getEndianIndex(i, Type.BYTES_PER_ELEMENT);
      dataView.setUint8(x, c);
   }
   
   return new Type(buffer);
}

var littleEndian =
   checkEndian() === "little endian";

function checkEndian() {
    var arrayBuffer = new ArrayBuffer(2);
    var uint8Array = new Uint8Array(arrayBuffer);
    var uint16Array = new Uint16Array(arrayBuffer);
    uint8Array[0] = 0xAA; // set first byte
    uint8Array[1] = 0xBB; // set second byte
    if(uint16Array[0] === 0xBBAA)
       return "little endian";
    if(uint16Array[0] === 0xAABB)
       return "big endian";
    else throw new Error("Something crazy just happened");
}

if (littleEndian) {
   getEndianIndex = function(i, bytesPerElement) {
         var count =
            Math.floor(i / bytesPerElement);
         var bytes = i % bytesPerElement;
         return count * bytesPerElement +
            bytesPerElement - bytes - 1;
      }
   }
else
   getEndianIndex = function(i) {
      return i;
   }



