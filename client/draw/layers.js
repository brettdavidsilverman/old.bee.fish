class Layers {
   
   constructor(input) {
   
      var layers = this;
      Object.assign(this, input);
   
      if (!input.stack)
         this.stack = [];
         
   }
   
   async top() {
      var stack = await this.stack.fetch();
      var top = stack[stack.length - 1];
      return top;
   }
 
   push(layer) {
      
      layer.layer = layer;
      
      setupMatricies(layer);
      
      this.stack.push(layer);
      
      return layer;
      
      async function setupMatricies(layer) {
  
         layer.transformMatrix = new Matrix();
         layer.inverseTransformMatrix = new Matrix();
         
         layer.matrix =
            canvas.initialMatrix;
   
         layer.inverse =
            layer.matrix.inverse();
            
      }
      

   }
   
   pop() {

      if (this.length == 0)
         return;

      return this.stack.pop();
      
   }
   
   draw(context) {
      
      for ( var index = 0;
                index < this.length;
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
         
         var dimensions =
            Dimensions.fromRectangle(
               canvas
            );

         dimensions.transform(
            layer.inverseTransformMatrix
         );
      
         return dimensions;
      }
   }
   
   get length() {
      return this.stack.length;
   }
      
}


