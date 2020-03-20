function Rectangle(input) {
   
   Object.assign(this, input);
   
   if (!this.strokeStyle)
      this.strokeStyle = "black";
 
   if (!isNaN(this.lineWidth))
      this.lineWidth = 0.5;
 
   if (!this.dimensions)
      this.dimensions = 
         Dimensions.fromRectangle(this);

   this.draw = function(context) {

      context.save();
      context.beginPath();
      context.strokeStyle = this.strokeStyle;
      var scale = context._scale;
      context.lineWidth =
         this.lineWidth / scale;
      context.fillStyle = this.fillStyle;
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
   
   this.hitTest = function(point) {
      if (this.dimensions
         .isPointInside(point))
         return this;
   }
   

}
