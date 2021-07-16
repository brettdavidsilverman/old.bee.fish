class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(input ? input.toolboxItem : null);


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
      var selection = this.canvas.selection;
      if (confirm("Delete " + selection.label + "?")) {
         selection.remove();
         this.canvas.selection = null;
         this.canvas.draw();
      }
   }
}
