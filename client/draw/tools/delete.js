class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(input);

   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "red";
      context.strokeStyle = "red";

      this.drawText(context, "", 40, "Arial", "X", true, true);

   }

   async click(point) {
      var selection = this.selection;
      if (confirm("Delete " + selection.label + "?")) {
         var parent = selection.parent;
         selection.remove();
         parent.save();
         this.selection = null;
         this.toolbox.parent.draw();
         this.toolbox.remove();
      }
   }
}
