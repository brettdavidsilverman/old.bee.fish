class Layers {
   
   constructor(input) {
   
      var layers = this;
      Object.assign(this, input);
   
      if (!input.stack)
         this.stack = [];
         
   }
   
   async top() {

      var stack = await this.stack;
      var topPointer = stack[stack.length - 1];
      var promise;
      if (Pointer.isPointer(topPointer))
         return topPointer.fetch();
      else
         return Promise.resolve(topPointer);
   }
 
   async push(layer) {
      
      layer.layer = layer;

      setupMatricies(layer);
      
      var stack = await this.stack;
      
      stack.push(layer);
      
      return layer;
      
      async function setupMatricies(layer) {
  
         layer.transformMatrix = new Matrix();
         layer.inverseTransformMatrix = new Matrix();
         
         layer.matrix =
            await canvas.initialMatrix;
         
         layer.inverse =
            layer.matrix.inverse();
            
      }
      

   }
   
   async pop() {

      if (this.length == 0)
         return;

      var stack = await this.stack;
      return stack.pop();
      
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
   
   async length() {
      var stack = await this.stack;
      return stack.length;
   }
      
}


