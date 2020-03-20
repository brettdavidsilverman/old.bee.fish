class BitStream {
   
   constructor(bits)
   {
      if (bits)
         this.bits = bits;
      else
         this.bits = [];
      this.index = 0;
      this.count = 0;
   }
   
   writeBit(bit) {

      this.bits.push(bit);
      
   }
   
   writeBits(bits) {
   
      this.bits.push( ...bits);
     
   }
   
   readBit() {
   
      if (this.index >= this.bits.length)
         throw "Past end of stream";
         
      var bit = this.bits[this.index++];
      
      if (bit == 0)
         this.count--;
      else
         this.count++;
      
      return bit;
   }
   
   readBits(length = 1) {

      var bits =
         this.bits.slice(
            this.index,
            this.index += length
         );
     
      return bits;
     
   }
   
   peek() {
      return this.bits[this.index];
   }
   
   
   get pastEnd() {
      return this.index >=
             this.bits.length;
   }
   
   
   toString16() {
   
      return String16.fromArrayBase2(
         this.bits
      );
      
   }
      
   
   reset() {
      this.index = 0;
   }

}

BitStream.fromString16 =
function(string16)
{
   var bits = string16.toArrayBase2();

   var stream = new BitStream(bits);

   return stream;

}


class String8 extends String {

   constructor(string) {
      super(string);
   }
   
   toString16() {
   
      var string = "";

      for ( var i = 0;
            i < this.length;
            i += 2 )
      {
         var charCodeHigh =
            this.charCodeAt(i);
            
         var charCodeLow =
            this.charCodeAt(i + 1);
            
         var char16 =
	            (charCodeHigh
	               << BYTE_SIZE) |
	            charCodeLow;
	            
         string +=
            String.fromCharCode(char16);
      }
      
      return new String16(string);
   }
   
   toBase64() {
      return btoa(this);
   }
}

String8.fromBase64 = function(string) {
   return new String8(
      atob(string)
   );
}

class String16 extends String {

   constructor(string) {
      super(string);
   }
   
   // Converts a string of 16 bit
   // characters to a string of 8 bit 
   // characters.
   // This is useful when using the
   // built in btoa() function.
   toString8() {

      var string = "";
      
      this.forEach(
         function(char16) {
            var uInt16 =
               char16.charCodeAt();
               
            var char8High = 
               (0xFF00 & uInt16)
               >> BYTE_SIZE;
            var char8Low =
               (0x00FF & uInt16);
            string +=
               String.fromCharCode(
                  char8High
               );
            string +=
               String.fromCharCode(
                  char8Low
               );
         }
      );
      
      return new String8(string);
   }
   
   // Converts a string to
   // an array of float 64 numbers
   // (64 bit numbers is javascript
   // basic numbers)
   toFloat64Array() {
   
      var string = this;
      
      var buffer =
         new ArrayBuffer(
            string.length * 16
            / BYTE_SIZE
         ); 
      
      var dataView = new DataView(
         buffer
      );

      for ( var i = 0;
            i < string.length;
            i++ )
      {
         var char16 =
            string.charCodeAt(i);
            
         dataView.setUint16(
            i * 16 / BYTE_SIZE,
            char16,
            true
         );
      }
   
   
      var array = new Float64Array(string.length * 16 / 64);

      for ( var i = 0, x = 0;
            i <= dataView.byteLength -
               64 / BYTE_SIZE;
            i += 64 / BYTE_SIZE )
      {
         var number =
            dataView.getFloat64(
               i,
               true
            );
      
         array[x++] = number;
      }
   
      return array;
   }
   
   write(stream) {
      var bits = this.toBits();
      bits.length.write(stream);
      stream.writeBits(bits);
   }
   
   get isAllChar8() {
      
      var isAllChar8 = true;
      
      this.forEach(
         function(char16) {
            var uInt16 =
               char16.charCodeAt();
            if (uInt16 >> 8)
               isAllChar8 = false;
         }
      );
      
      return isAllChar8;
   }
}

String16.fromArrayBase2 = function(bits) {

   var string = "";
   
   for ( var i = 0;
         i < bits.length;
       )
   {
      var charCode = 0;
      
      for ( var m = 16 - 1;
            m >= 0 && i < bits.length;
            m--, i++)
      {
         var bit = bits[i];
         charCode |= (bit << m);
      }
      
      string +=
         String.fromCharCode(charCode);
   }
  
   return new String16(string);
}

String16.readBlob = function(stream) {
   var length = Number.read(stream);

   var bits =
      stream.readBits(length);

   return String16.fromArrayBase2(bits);
}


// Returns an array of bits
// for each character in the string
String.prototype.toArrayBase2 =
function(length)
{
   if (!length)
      length = this.length * 16;
      
   var bits =
      new Array(length);
         
   // for all charactrrs
   var x = 0;
   this.forEach(
      function(char16) {

         // get the 16 bit chsracter
         // code from the character
         charCode16 =
            char16.charCodeAt();
            
         // for all bits in the
         // character code
         for ( var m = 16 - 1;
               m >= 0;
               m-- )
         {
            // create the bit mask
            var mask = 1 << m;
            
            // get the bit
            var bit = 
               (charCode16 & mask) ?
               1 : 0;
            
            // store the bit and
            // move the x index to the
            // next position
            bits[x++] = bit;
            
         }
      }
   );
   
   return bits;
}


Object.prototype.toString = function() {
   return JSON.stringify(this);
}


const BYTE_SIZE = 8;


// converts an array of 64 bit numbers
// to a string of 16 bit chatacters
Float64Array.prototype.toString16 = function()
{
   
   var bytes =
      this.length *
                64 /
         BYTE_SIZE;
            
   var arrayBuffer = 
      new ArrayBuffer(bytes);
            
   var dataView = 
      new DataView(arrayBuffer);

   for ( var i = 0; 
         i < this.length;
         i++ )
   {
      dataView.setFloat64(
         i * 64 / BYTE_SIZE,
         this[i],
         true
      );
   }

   var string = "";
   
   for ( var i = 0;
         i < dataView.byteLength;
         i += 16 / BYTE_SIZE )
   {
      var code = dataView.getUint16(
         i,
         true
      );
               
      string +=
         String.fromCharCode(
            code
         );
   }
         
   return new String16(string);
}


String.prototype.forEach = function(f) {
   return Array.from(this).forEach(f);
}


String.prototype.writeBlob = function(stream)
{
   var bits = this.toArrayBase2();
   bits.length.write(stream);
   stream.writeBits(bits);
}

String.prototype.write = function(stream) {

   this.forEach(
      function(char16) {
         char16.charCodeAt().write(
            stream
         )
      }
   );
   
   stream.writeBit(0);
   
}

String.read = function(stream) {

   var string = "";
   
   while (stream.peak() != 0)
   {
      var charCode =
         Number.read(stream);
      string +=
         String.fromCharCode(charCode);
   }
   
   stream.readBit();
   
   return string;
}


// Power Encoding

Number.BASE = 5;

// number = BASE^power + remainder

// define the power of a number is the
// highest integral exponent that
// leaves the smallest integral
// remainder
function power(number) {
   return Math.floor(
      Math.log(number) /
      Math.log(Number.BASE)
   );
}

// remainder = number - BASE^power
function remainder(number) {
   var power = 
      Number.BASE ** number.power;
            
  var remainder =
      number - power;
         
  return remainder;

}

Object.defineProperty(
   Number.prototype,
   "power",
   {
      get: function() {
         return power(this);
      }
   }
);

Object.defineProperty(
   Number.prototype,
   "remainder",
   {
      get: function() {
         return remainder(this);
      }
   }
);

// recurse for all numbers > 0
// creating a binary tree of
// power and remainder
Number.prototype.write = function(stream) {

   if (this == 0) {
      // closed branch
      stream.writeBit(0);
      return;
   }
      
   if (this == 1) {
      // open branch with both
      // leaves closed
      stream.writeBits([1,0,0]);
      return;
   }
      
   // open branch
   stream.writeBit(1);
   
   // left branch
   this.power.write(stream);
   // right branch
   this.remainder.write(stream);
   
}

Number.read = function(stream) {

   // read the next bit
   var bit = stream.readBit();
  
   if (bit == 0)
      return 0;
   
   // bit == 1 defines an open branch,
   
   // read the left branch (power)
   var power = Number.read(stream);
   
   // read the right branch (remainder)
   var remainder = Number.read(stream);
   
   // compose the number as
   // BASE^power + remainder
   var number =
      Number.BASE ** power + remainder;
      
   return number;
}

Number.prototype.encode = function() {
   var stream = new BitStream();
   this.write(stream);
   return stream;
}

class Types {
}

var types = new Types();

Types.NULL = (1).encode();
Types.UNDEFINED = (2).encode();
Types.NUMBER = (3).encode();
Types.STRING = (4).encode();
Types.ARRAY = (5).encode();
Types.OBJECT = (6).encode();