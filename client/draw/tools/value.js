class Value extends ToolboxItem {
   canvas;

   constructor(input) {
      super(
         {
            topLeft: {x: 230, y: 10},
         }
      );

      this.canvas = input.canvas;
      this.label = "𝓧";
      this.fontSize = 40;
      this.fontFamily =  "Arial italic";
   }

   async draw(context) {

      await super.draw(context);

      context.pushMatrix(context);
      
      context.lineWidth = 1;
      context.fillStyle = "blue";
      context.textAlign    = 'center';
      context.textBaseline = 'middle';
      context.font = this.fontSize + "px " + this.fontFamily;

      var dim = this.dimensions;

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);

      this.popMatrix(context);
   }

   async click(point) {

      var selection = this.canvas.selection;

      var value = selection.value;
      
      if (value == null)
         value = "x";
      
      value = prompt("Value", value);
      
      if (value == null)
         return;

      selection.value = value;
      selection.save();
      this.canvas.draw();
   }

}
