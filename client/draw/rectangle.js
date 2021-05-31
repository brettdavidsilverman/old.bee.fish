class Rectangle {

   constructor(input) {
   
      Object.assign(this, input);
   
      if (!input.strokeStyle)
         this.strokeStyle = "black";
 
      if (!input.fillStyle)
         this.fillStyle = "yellow";
         
      if (isNaN(input.lineWidth))
         this.lineWidth = 0.5;
 
      if (!input.dimensions)
         this.dimensions = 
            Dimensions.fromRectangle(this);

   }
   
   
   async draw(context) {
   
      context.save();
      context.beginPath();
      var scale = context._scale;
      
      context.lineWidth =
         this.lineWidth / scale;
         
      context.fillStyle = this.fillStyle;
      context.strokeStyle = this.strokeStyle;
      
      var dimensions = await this.dimensions;
      var topLefy = await dimensions.topLeft;
      
      context.rect(
         topLeft.x,
         topLeft.y,
         dimensions.width,
         -dimensions.height
      );
      
      if (this.fillStyle) 
         context.fill();
         
      if (this.strokeStyle)
         context.stroke();
         
      context.restore();

   }
   
   async hitTest(point) {
      var dimensions = await this.dimensions;
      
      if (dimensions.isPointInside(point))
         return this;
   }
   

}
