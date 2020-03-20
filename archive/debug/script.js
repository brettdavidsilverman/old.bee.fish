/*
Debug displays messages to help you understand your running code.


input:
   element:
      The element that displays the debug information
   
   displayCount:
      The number of debug messages to display 
      
   filter:
      An array of strings that only display if the names match. Optional.

Example:

   var debug = new Debug(
      {
         element: debugElement,
         displayCount: 10,
         filter: ["name", "age"]
      }
   );


*/

function Debug(input) {

   if (input && input.id)
      this.id = input.id;
   else
      this.id = null;
      
   var element = input.element;
   if (!element) {
      element =
         document
         .createElement("pre");
      element.id = input.id;
      document.body.appendChild(
         element
      );
   }
   this.displayCount =
      input.displayCount ?
         input.displayCount :
         20;
         
   this.filter =
      input.filter ?
         input.filter :
         [];
   
   this.items = [];

   var debugCount = 0;
   
   this.add = function(name, message) {
      if (this.filter.indexOf(name) == -1
         ) {
         
         var item = new Item(name, message);

         this.items.push(item);
     
         var count;
   
         if (this.items.length < this.displayCount)
            count = this.items.length;
         else
            count = this.displayCount;

         if (count == 0)
            return;
  
         var start = this.items.length - count;
   
         var select = this.items.slice(start);
   
         select = select.reverse();
         
         element.innerText = select.join("\n");
      }
   }
   
   function Item(name, message) {
   
      this.name = name;
      this.message = message;
      this.timeStamp = new Date();
      this.count = ++debugCount;
  
      var str = String(this.count);

      str += " " + this.timeStamp.toLocaleTimeString();

      str += " " + name;
      
      if (typeof(message) != "undefined") {
         str += "\n" + String(message);
      }
  
      this.toString = function() {
         // return Strinput.(message);
         return str;
      }
      
   }

}

Debug.catch = function(error) {
   
   var output = {}
   
   
   if ("name" in error)
      output.name = error.name;
   else
      output.name = String(error);
      
   if ("stack" in error)
      output.message = error.stack;

   if (debug)
      debug.add(output.name, output.message);
   
   alert(output.name + "\n" + output.message);

}
