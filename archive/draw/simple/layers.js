class Layers extends Storable {
   
   constructor(
      input = {
         stack: [],
         canvas: undefined
      }
   ) {
      super(input);
      
      if (!input.stack)
         this.stack = [];
   
      if (this.length == 0) {
         var editor = new App(
            {
               canvas: this.canvas
            }
         );
         var layer = new Layer(
            {
               editor,
               canvas: this.canvas,
               zIndex: 0
            }
         );
         this.push(layer);
      }
 
   }
   
   
   get current() {
      return this.stack[
         this.stack.length - 1
      ];
   }
   
   push(layer) {
 
      var zIndex = this.length;
      var canvas = this.canvas;

      if (this.current)
         this.current.matrix = 
            [...this.canvas.matrix];
         
      this.stack.push(layer);

      // reset the canvas' matrix
      this.canvas.matrix =
         [...this.canvas.initialMatrix];


      return layer;
   }
   
   pop() {

      var layer = this.stack.pop();
      
      // restore the canvas matrix
      this.canvas.matrix =
         this.current.matrix;
      
      this.save();

      return layer;
   }
   
   hitTest(point) {
      
      var hit = 
         this
         .current
         .hitTest(point);
         
      return hit;
   }
   
   draw() {
   
      var layers = this;
      var blur = 1;
      
      this.stack.forEach(
         layer => drawLayer(
            layer, blur++
         )
      );
   
      
      function drawLayer(layer, blur) {
         
         var context =
            canvas.context;
               
         context.globalAlpha =
            blur /
            layers.length;
               
         layer.draw(context);
           
      }
   }
   
   get length() {
      return this.stack.length;
   }
}

class Layer extends Storable {
   constructor(
      input = {
         zIndex: canvas.layers.length,
         editor: undefined,
         canvas: undefined
      }
   ) {
      super(input);
      
      if (!this.canvas) {
         this.canvas = canvas;
      }
      if (isNaN(this.zIndex)) {
         this.zIndex =
            this.canvas.layers.length;
      }

      this.editor.layer = this;
   }
   
   hitTest(point) {
      var hit =
         this.editor.hitTest(point);
      return hit;
   }
   
   draw(context) {
      var matrix = this.matrix;
      
      if (!matrix)
         matrix = canvas.matrix;
     
      context.applyMatrix(matrix);
      this.editor.draw(context);
   }
   
}
