class FunctionTool extends ToolboxItem {

   form;
   editing = false;

   constructor(input) {
      super(input);

      this.label = "f(x)";
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

      this.editing = !this.editing;

      if (selection.form == undefined) {
         // Create the form
         selection.form = new Form(
            {
               canvas: this.toolbox.parent,
               item: selection
            }
         );
      }
     
      if (this.editing) {
         selection.form.div.style.zIndex = "3";
         selection.form.div.contentEditable = true;
         selection.form.div.focus();
      }
      else {
         selection.form.div.contentEditable = false;
         selection.form.div.style.zIndex = "0";
      }

      this.toolbox.draw();
   }

   remove() {
      if (this.selection && this.selection.form) {
         this.selection.form.div.style.zIndex = "0";
         this.selection.form.div.contentEditable = false;
      }
      super.remove();
   }


}
