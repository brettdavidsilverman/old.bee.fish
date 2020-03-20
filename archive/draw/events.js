function Events() {
   var events = [];
   this.add = function(f) {
      events.push(f);
   }
   
   this.call = function(value) {
         events.forEach(
            function(f) {
               f(value);
            }
         )
   }
   
   return this;
}
