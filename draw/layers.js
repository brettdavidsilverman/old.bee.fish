class Layers {
   
   constructor(input) {
   
      var layers = this;
      Object.assign(this, input);
   
      if (input.stack == null)
         this.stack = [];
          
      if (input.index == null)
         this.index = -1;
         
   }
   
   get top() {
      return this.stack[this.index];
   }
 
   push() {
 
      var canvas = this.canvas;
      var layer;
      
      ++this.index;

      if (this.index == 0) {
         // Blank drawing
         layer = new Drawing(
            {
               canvas
            }
         );
         layer.layer = layer;
         this.stack.push(layer);
      }
      else if (this.index >= this.stack.length) {
         // Editor
         layer = new Editor(
            {
               canvas
            }
         );
         layer.layer = layer;
         this.stack.push(layer);
      }
      else
         layer = this.top;
     
      setupMatricies(layer);
      
      this.save()
    
      return layer;
      
      function setupMatricies(layer) {
  
         layer.transformMatrix =
            glMatrix.mat2d.create();
         layer.inverseTransformMatrix =
            glMatrix.mat2d.create();
      
      
         layer.matrix =
            glMatrix.mat2d.clone(
               canvas.initialMatrix
            );
         layer.inverse =
            glMatrix.mat2d.create();
         
         glMatrix.mat2d.invert(
            layer.inverse,
            layer.matrix
         );
      }
      

   }
   
   pop() {

      if (this.index == 0)
         return;
         
      --this.index;

      var layer = this.top;
      
      this.save();
      
      return layer;
   }
   
   draw(context) {
      
      for ( var index = 0;
                index <= this.index;
                ++index) {
         var layer = this.stack[index];
         drawLayer(
            layer, (index + 1) / this.length
         );
      }
   
      
      function drawLayer(layer, blur) {
               
         context.globalAlpha = blur;
         
         context.applyMatrix(layer.matrix);

         context.dimensions =
            getDimensions(layer);
            
         layer.draw(context);
           
      }
   
      function getDimensions(layer) {
         var canvas = layer.canvas;
         
         var dimensions = new Dimensions(
            {
               min: new Point(
                  {
                     x: -canvas.width / 2,
                     y: -canvas.height / 2
                  }
               ),
               max: new Point(
                  {
                     x: canvas.width / 2,
                     y: canvas.height / 2
                  }
               )            
            }
         );
         
         dimensions.transform(
            layer.inverseTransformMatrix
         );
      
         return dimensions;
      }
   }
   
   get length() {
      return this.index + 1;
   }
      
}


