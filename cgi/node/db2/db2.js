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
