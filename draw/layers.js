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
 
   push(layer) {
      
      ++this.index;
      layer.layer = layer;

      this.stack.push(layer);
      
      var canvas = this.canvas;
      
      setupMatricies(layer);
      
      return layer;
      
      function setupMatricies(layer) {
  
         layer.transformMatrix = new Matrix();
         layer.inverseTransformMatrix = new Matrix();
         
         layer.matrix =
           Matrix.fromMatrix(
              canvas.initialMatrix
           );
         layer.inverse =
            layer.matrix.inverse();
      }
      

   }
   
   pop() {

      if (this.index == 0)
         return;
         
      --this.index;

      var layer = this.top;
      
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


