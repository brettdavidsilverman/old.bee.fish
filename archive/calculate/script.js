var time = null;
var Calculate = {}

Function.prototype.valueOf = function() {

   for ( var obj = this;
         typeof obj == "function";
         obj = obj.call(Calculate.object, time)
       )
      ;
      
   return obj;
}


function isNull(obj) {
   return (obj == null) ||
          (obj == undefined) ||     
          (obj.valueOf() == null) ||
          (obj.valueOf() == undefined);
}

function Bool(obj) {
   return Number(obj) == 1;
}