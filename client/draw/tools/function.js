class FunctionTool extends ToolboxItem {

   form;
   editing = false;

   constructor(input) {
      super(input);

      this.label = "f(x)";

      if (this.selection instanceof Form) {
         this.form = this.selection;
      }
   }

   async draw(context) {

      await super.draw(context);

      context.lineWidth = 1;
      context.fillStyle = "yellow";
      context.textAlign    = "center";
      context.textBaseline = "middle";
      context.font = "italic 40px Times New Roman";

      var dim = this.dimensions;

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);

   }

   async click(point) {
      var selection = this.selection;

      if (this.form == undefined) {
         // Create the form
         this.form = new Form(
            {
               item: {
                  parent: selection,
                  dimensions: selection.dimensions
               }
            }
         );
         this.form.label = selection.label + " form";
         selection.children.push(this.form);

      }
     
      this.form.editing = !this.form.editing;

      this.toolbox.draw();
      this.toolbox.parent.draw();
   }

   remove() {
      if (this.form) {
         this.form.editing = false;
      }
      super.remove();
   }


}
