class Rectangle {

   constructor(input) {
   
      Object.assign(this, input);
   
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";
         
      if (!isNaN(this.lineWidth))
         this.lineWidth = 0.5;
 
      if (!this.dimensions)
         this.dimensions = 
            Dimensions.fromRectangle(this);

   }
   
   
   draw(context) {

      context.save();
      context.beginPath();
      var scale = context._scale;
      
      context.lineWidth =
         this.lineWidth / scale;
         
      context.fillStyle = this.fillStyle;
      context.strokeStyle = this.strokeStyle;
      
      var dimensions = this.dimensions;
      context.rect(
         dimensions.topLeft.x,
         dimensions.topLeft.y,
         dimensions.width,
         -dimensions.height
      );
      
      if (this.fillStyle) 
         context.fill();
         
      if (this.strokeStyle)
         context.stroke();
         
      context.restore();

   }
   
   hitTest(point) {
      if (this.dimensions
         .isPointInside(point))
         return this;
   }
   

}
