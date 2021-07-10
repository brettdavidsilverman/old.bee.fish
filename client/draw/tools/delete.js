class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(
         Object.assign(
            input,
            {
               topLeft: {x: 10, y: 10},
            }
         )
      );


   }

   async draw(context) {

      await super.draw(context);

      context.pushMatrix(context);
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
      
      this.popMatrix(context);
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