class Rectangle extends Line {

   constructor(input) {
      super(input ? input : {} );
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";

   }
   
   
   async draw(context) {

      context.beginPath();
      
      var dimensions = this.dimensions;
      
      
      if (this.fillStyle) {
         context.fillStyle = this.fillStyle;
         context.fillRect(
            dimensions.min.x,
            dimensions.min.y,
            dimensions.width,
            dimensions.height
         );
            
      } 

      if (this.strokeStyle) {
         context.strokeStyle = this.strokeStyle;
         context.lineWidth = this.lineWidth / context.matrix.scale();
         context.strokeRect(
            dimensions.min.x,
            dimensions.min.y,
            dimensions.width,
            dimensions.height
         );
      }

      return true;
         
   }
   


}
