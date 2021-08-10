class FunctionTool extends ToolboxItem {

   form;
   editing = false;

   constructor(input) {
      super(input);

      if (this.selection instanceof Form) {
         this.form = this.selection;
      }
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "yellow";
      context.strokeStyle = "yellow";

      this.drawText(context, "Italic 40px Times New Roman", "f(x)", true);

   }

   async click(point) {
      var selection = this.selection;

      if (this.form == undefined) {
         // Create the form
         var parent = selection.parent;
         var form = new Form(
            {
               item: selection
            }
         );
         form.label = selection.label + " form";
         parent.children.push(form);
         selection.remove();
         form.save();
         parent.save();
         this.form = form;
         this.form.editing = true;
      }
      else {
         this.form.editing = !this.form.editing;
      }

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
