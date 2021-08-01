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
         dim.min.x + 1, 
         dim.min.y + 1
         );

      context.lineTo(
         dim.max.x - 1,
         dim.max.y - 1
      );

      context.moveTo(
         dim.max.x - 1, 
         dim.min.y + 1
      );

      context.lineTo(
         dim.min.x + 1,
         dim.max.y - 1
      );

      context.stroke();
      
   }

   async click(point) {
      var selection = this.canvas.selection;
      if (confirm("Delete " + selection.label + "?")) {
         selection.remove();
         this.canvas.selection = null;
         this.canvas.toolbox.remove();
         this.canvas.draw();
      }
   }
}
