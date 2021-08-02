class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(input);


   }

   async draw(context) {

      await super.draw(context);

      context.lineWidth = 2;
      context.strokeStyle = "red";

      var dim = this.dimensions;

      context.moveTo(
         dim.min.x, 
         dim.min.y
         );

      context.lineTo(
         dim.max.x,
         dim.max.y
      );

      context.moveTo(
         dim.max.x, 
         dim.min.y
      );

      context.lineTo(
         dim.min.x,
         dim.max.y
      );

      context.stroke();
      
   }

   async click(point) {
      var selection = this.selection;
      if (confirm("Delete " + selection.label + "?")) {
         selection.remove();
         this.selection = null;
         this.toolbox.parent.save();
         this.toolbox.parent.draw();
         this.toolbox.remove();
      }
   }
}
