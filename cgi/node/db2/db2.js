if (typeof BitString === "undefined")
   BitString = require("../../../power-encoding/bit-string/bit-string.js")


class DB2 extends Array {
   static left = "0";
   static right = "1";
   _last;
   pointer;
   
   constructor(...args) {
      super(...args);
      this._last = this.length;
      this.pointer = 0;
   }
   
   walkPath(bits) {
      var bit;
      for(var i = 0; i < bits.length; i++)
      {
         bit = bits[i];
         this.walkBit(bit);
      }
      
      return this.pointer; 
   }
   
   walkBit(bit) {
   
      var index = this.pointer;
      
      if (bit === DB2.right)
         ++index;
    
      if (this[index] === undefined) {
         this._last += 2;
         this[index] = this._last;
      }
      
      return this.pointer = this[index];
   }
   
   traverse(stream, pointer = 0) {
      var index = pointer;
      if (this[index]) {
         stream.write("1");
         this.traverse(
            stream, this[index]
         );
      }
      else
         stream.write("0");
         
      if (this[index + 1]) {
         stream.write("1");
         this.traverse(
            stream, this[index + 1]
         );
      }
      else
         stream.write("0");
   }
   
   traverseQuick(stream, pointer = 0) {
      var stack = [];
      var index = pointer;
      stack.push(index);
      
      // traverse left
      while (stack.length) {
      
         var index = stack.pop();
         
        // stream.write("1");
         
         var leftBranch = this[index];
         var rightBranch = this[index + 1];
         
         // add the right branch
         if (rightBranch)
            stack.push(rightBranch);
            
         // add the left branch
         // and write ro stream
         if (leftBranch) {
            stack.push(leftBranch);
            stream.write("1");
         }
         else
            stream.write("0");
         
         // add the right branch
         if (rightBranch)
            stream.write("1");
         else
            stream.write("0");
         
      }
   }
   /*
   next(count = 10) {
      var items = new Array(count);
      var found = 0;
      var index = pointer;
      var stack = ["0", index];

      var bits = [];
      var bit;
      
      while (stack.length &&
             found < count)
      {
         var trace = stack.pop();
         
         bit = trace[0];
         index = trace[1];
         bits.push(bit);
         
         var leftBranch = this[index];
         var rightBranch = this[index + 1];
         
         
         if (rightBranch) 
            stack.push(["1", rightBranch]);
         
         if (leftBranch)
            stack.push(["0", leftBranch]);
         
         if (!leftBranch && !rightBranch) {
            var bitString = new BitString(
               {
                  bits: bits.join(""),
                  bitsPerCharacter: 8
               }
            );
            var string =
               bitString.toString();
            items.push(string);
            found++;
         }
      }
      
      return items;
   }
   */
   next(input) {
   
      var count = input.count;
      var items = input.items;
      var index = input.index;
      var bits = input.bits;
      
      if (count == 0)
         return items;
      
      var leftBranch = this[index];
      var rightBranch = this[index + 1];
      
      if (leftBranch) {
         this.next(
            {
               count,
               items,
               index: leftBranch, 
               bits: bits + "0"
            }
         );
         
         if (items.length == count)
            return items;
      }
         
      if (rightBranch) {
         this.next(
            {
               count,
               items,
               index: rightBranch, 
               bits: bits + "1"
            }
         );
         
         if (items.length == count)
            return items;
      }
      
      if (!leftBranch && !rightBranch) {
         var bitString = new BitString(
            {
               bitsPerCharacter: 8,
               bits: bits.substr(
                  0,
                  bits.length - 1
               )
            }
         );
         var string =
            bitString.toString();
         items.push(string);
      }
      
   }
   
   toString() {
      var str = "";
      for ( var index = 0;
            index < this.length;
            index += 2 )
      {
         str += "[" +
            (this[index] ? "+" : "-") + "," +
            (this[index + 1] ? "+" : "-") +
            "]\r\n";
            
      }
      
      return str;
   }
}

module.exports = DB2;
