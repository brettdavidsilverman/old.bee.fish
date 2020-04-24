function Layers(input) {
   
   var layers = this;
   Object.assign(this, input);
   
   if (input.stack == null)
      this.stack = [];
   
   if (input.index == null)
      this.index = -1;
      
   this.getTop = function() {
      return this.stack[this.index];
   }
 
   Object.defineProperty(
      this,
      "top",
      {
         get: this.getTop
      }
   );
  
   this.push = function() {
 
      var canvas = this.canvas;
      var layer;
      
      ++this.index;

      if (this.index == 0) {
         // Blank drawing
         layer = new Drawing(
            {
               canvas: this.canvas
            }
         );
         layer.layer = layer;
         this.stack.push(layer);
      }
      else if (this.index >= this.stack.length) {
         // Editor
         layer = new Editor(
            {
               canvas: this.canvas
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
   
   this.pop = function() {

      if (this.index == 0)
         return;
         
      --this.index;

      var layer = this.top;
      
      layer.selected = false;
      
      this.save();
      
      return layer;
   }
   
   this.draw = function(context) {
      
      for ( var index = 0;
                index <= this.index;
                ++index) {
         var layer = this.stack[index];
         drawLayer(
            layer, (index + 1) / layers.length
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
         var canvas = layers.canvas;
         
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
   
   this.getLength = function() {
      return this.index + 1;
   }
   
   Object.defineProperty(
      this,
      "length",
      {
         get: this.getLength
      }
   );
      
}


