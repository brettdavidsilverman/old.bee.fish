class Rectangle extends Line {

   constructor(input) {
   
      super(input);
      
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";

   }
   
   
   async draw(context) {

      if (!this.clipContext(context))
         return false;
      
      context.beginPath();
      
      context.fillStyle = this.fillStyle;
      context.strokeStyle = this.strokeStyle;
      var dimensions = this.dimensions;
      
      context.rect(
         dimensions.topLeft.x,
         dimensions.topLeft.y,
         dimensions.width,
         dimensions.height
      );
      
      if (this.fillStyle) 
         context.fill();
         
      context.stroke();

      context.popStack();
      
      return true;
         
   }
   


}
