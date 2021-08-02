class ValueTool extends ToolboxItem {

   constructor(input) {
      super(input);

      this.label = "𝓧";
   }

   async draw(context) {

      await super.draw(context);

      context.lineWidth = 1;
      context.fillStyle = "blue";
      context.textAlign    = "center";
      context.textBaseline = "middle";
      context.font = "40px Fixed Sys";

      var dim = this.dimensions;

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);
   }

   async click(point) {


      var selection = this.selection;

      var value = selection.value;
      
      if (value == null)
         value = "𝓧";
      
      value = prompt("Value", value);
      
      if (value == null)
         return;

      selection.value = value;
      selection.save();
      this.toolbox.parent.draw();
      this.toolbox.remove();
   }

}
