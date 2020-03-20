Object.prototype.round = function(precision) {
   
   if (typeof(this) == "number")  {
      let pow = Math.pow(10, precision);
      return Math.round(this * pow) / pow;
   }
   else if (Array.isArray(this)) {
      var out = new Array(this.length);
      for (var i = 0; i < out.length; i++) {
         out[i] = this[i].round(precision);
      }
      return out;
   }
   else
      return this;
}

Array.prototype.remove = function() {

   var index = this.indexOf(this);
   
   if (index == -1)
      return this;
      
   var head = this.slice(0, index);

   var tail = this.slice(index + 1);
      
   var array = head.concat(tail);

   return array;
}