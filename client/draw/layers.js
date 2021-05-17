class Layers {
   
   constructor(input) {
   
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

      await setupMatricies(layer);
      
      var stack = await this.stack;
      
      stack.push(layer);
      
      return layer;
      
      async function setupMatricies(layer) {
         
         
         layer.transformMatrix = new Matrix();
         layer.inverseTransformMatrix = new Matrix();
         
         var matrix = await canvas.initialMatrix;
         
         layer.matrix = matrix;

         layer.inverse = matrix.inverse();
            
         //console.log("setupMatrices: " + layer.matrix );
      }
      

   }
   
   async pop() {

      if (this.length == 0)
         return;

      var stack = await this.stack;
      return stack.pop();
      
   }
   
   async draw(context) {
      
      var length = await this.length();
      var stack = await this.stack;
      
      for ( var index = 0;
                index < length;
                ++index) {
         var layer = stack[index];
        
         drawLayer(
            layer, (index + 1) / length
         );
      }
   
      
      async function drawLayer(layer, blur) {
        // console.log("Layers.drawLayer.1");

         context.globalAlpha = blur;
         
         var matrix = await layer.matrix;
         //console.log("Layers.drawLayer.layer.matrix " + matrix);
         context.applyMatrix(matrix);

         context.dimensions =
            getDimensions(layer);
            
         layer.draw(context);
        // console.log("Layers.drawLayer.2.dimensions " + context.dimensions);
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


