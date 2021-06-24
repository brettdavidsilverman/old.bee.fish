class Rectangle extends Line {

   constructor(input) {
   
      super(input);
      
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";

   }
   
   
   draw(context) {

      context.beginPath();
      
      context.fillStyle = this.fillStyle;
 
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
         
   }
   


}
