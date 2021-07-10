class LabelTool extends ToolboxItem {

   constructor(input) {
      super(
         Object.assign(
            input,
            {
               topLeft: {x: 120, y: 10},
            }
         )
      );

      this.label = "abc";
   }

   async draw(context) {

      await super.draw(context);

      context.pushMatrix(context);
      
      context.lineWidth = 1;
      context.fillStyle = "green";
      context.textAlign    = 'center';
      context.textBaseline = 'middle';
      context.font = "40px Arial";

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

      var label = selection.label;

      if (label == null)
         label = "label";
      
      label = prompt("Label", label);
      
      if (label == null)
         return;

      selection.label = label;
      selection.save();

      this.canvas.draw();
   }

}
