class Stack extends Id {
   
   stack = [];

   constructor(input) {
      super(input);
      var stack = this.stack;

      if (input == undefined)
         input = {}

      if (input.stack) {
         input.stack.forEach(
            async function(key) {
               var canvas = await Id.load(key);
               stack.push(canvas);
            }
         );
      }

      if (stack.length == 0) {
         stack.push(new Canvas()); 
      }
   }

   toJSON() {
      return {
         ms: super.ms,
         inc: super.inc,
         stack: this.stack.map(
            canvas => canvas.key
         )
      }
   }
   
   async save() {
      super.save();
      this.stack.forEach(
         canvas => canvas.save()
      );
   }

   get top() {
      return this.stack[this.length - 1];
   }
 
   push(canvas) {
      this.stack.push(canvas);
      this.save();
      this.draw();
      console.log("Push: " + window.stack.length);
   }
   
   pop() {

      if (this.length <= 1)
         return;

      console.log("Pop: " + window.stack.length);

      var canvas = this.stack.pop();

      this.save();
      this.draw();

      return canvas;
   }
   
   draw() {

      var layers = this;

      window.requestAnimationFrame(
   
         function(timestamp) {
               
            if ( timestamp <=
                 layers._lastDrawTimestamp
               )
            return;
         
            layers._lastDrawTimestamp =
               timestamp;
       
            draw().catch(error => console.log(error.stack));
         }
      );

      async function draw() {

         var stack = layers.stack;
         console.log("Draw: " + window.stack.length);

         for ( var index = 0;
                  index < stack.length;
                  ++index) {
            var canvas = stack[index];
            var blurDepth = (stack.length - index - 1) * 10;

            await drawCanvas(
               canvas,
               0
            );
         }
   
         async function drawCanvas(canvas, blurDepth) {

            var context = canvas.context;

            if (blurDepth > 0)
               context.filter = "blur(" + blurDepth + "px)";
            else 
               context.filter = ""; 

            await canvas.draw();

         }
      }
   }
   
   get length() {
      return this.stack.length;
   }

   get selection() {

      var canvas;

      if (this.stack.length < 2)
         canvas = this.stack[0];
      else 
         canvas = this.stack[this.stack.length - 2];

      return canvas.selection;
   }
      

   static async load()
   {
      var key = await
         storage.getItem("Stack");

      if (key)
      {
         console.log("Fetching Stack");
         var id = Id.fromKey(key);
         window.stack = await id.load();
         console.log("Load: " + window.stack.length);
      }
      
      if (window.stack == undefined)
      {
         console.log("Creating new stack");
         window.stack = new Stack();
         window.stack.save();
         storage.setItem("Stack",  window.stack.key);
      }
      
      return window.stack;

   }
}