class Line extends Item {
   
   strokeStyle = "blue";
   lineWidth = 1.0;
   points;
   
   constructor(input) {
      super(input ? input.item : null);

      if (!input)
         input = {}
        
      if (input.points == undefined)
         this.points = new Points();
      else
         this.points =
            new Points(...input.points);

      if (input.strokeStyle)
         this.strokeStyle = input.strokeStyle;
   }
  
   toJSON()
   {
      return {
         item: super.toJSON(),
         strokeStyle: this.strokeStyle,
         lineWidth: this.lineWidth,
         points: this.points
      }
   }
   
   async draw(context) {
      
      var draw = await super.draw(context);

      if (draw) {
         context.save();

         var scale = context.matrix.scale();

         var lineWidth =
            this.lineWidth / scale;
         
         context.lineWidth = lineWidth;
         
         context.strokeStyle = this.strokeStyle;
         
         this.points.draw(context);

         context.restore();
      }

      return draw;
      
   }

   
   

}
