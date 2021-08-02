class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(input);

      this.label = "X";

   }

   async draw(context) {

      await super.draw(context);

      context.lineWidth = 1;
      context.fillStyle = "red";
      context.textAlign    = "center";
      context.textBaseline = "middle";
      context.font = "40px Arial";

      var dim = this.dimensions;
      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

<<<<<<< HEAD
      context.fillText(this.label, point.x, point.y);
=======
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
>>>>>>> e9ee9814d12429ef1bc2f6c72b276d4463a79323

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
