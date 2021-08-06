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

      context.fillText(this.label, point.x, point.y);

   }

   async click(point) {
      var selection = this.selection;
      if (confirm("Delete " + selection.label + "?")) {
         selection.remove();
         this.selection = null;
         this.toolbox.parent.draw();
         this.toolbox.remove();
      }
   }
}
