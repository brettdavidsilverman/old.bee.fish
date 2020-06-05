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
      
      this.walkBit("0");
      
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
