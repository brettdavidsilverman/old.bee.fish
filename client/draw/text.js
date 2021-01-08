function Text(input) {

   Line.call(this, input);
   
   this.draw = function(context) {
      this.drawText(context);
      if (this.selected)
         this.drawFrame(context);
   }
   
   
   this.drawText = function(context) {
      
      context.save();
      
      context.fillStyle = "black";
      context.strokeStyle = "black";
      
      var height = 3;

      var scale = context._scale;
      var fontSize = height; //mm
      
      context.font =
         String(fontSize) + "mm" +
         " Arial Bold";
     
      var width = context.measureText(
         this.text
      ).width;
      
      context.scale(1, -1);
      
      context.beginPath();
         
      context.fillText(
         this.text,
         this.point.x,
         -this.point.y
      );
      context.stroke();
      
      this.dimensions.min =
         new Point(
            {
               x: this.point.x,
               y: this.point.y
            }
         );
         
      this.dimensions.max =
         new Point(
            {
               x: this.point.x + width,
               y: this.point.y + height * 2
            }
         );
     
      context.restore();
   }
   
   this.click = function(point) {
      var text = prompt("Text?", this.text);
      if (text == null)
         return false;
      this.text = text;
      return true;
   }

}