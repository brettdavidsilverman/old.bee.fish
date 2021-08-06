class FunctionTool extends ToolboxItem {

   form;
   editing = false;

   constructor(input) {
      super(input);

      this.label = "f(x)";

      if (this.selection instanceof Form)
         this.form = this.selection;
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

      if (this.form == undefined) {
         // Create the form
         this.form = new Form(
            {
               item: selection
            }
         );
         selection.children.push(this.form);

      }
     
      if (this.editing) {
         this.form.div.style.zIndex = "3";
         this.form.div.contentEditable = true;
         this.form.div.focus();
      }
      else {
         this.form.div.contentEditable = false;
         this.form.div.style.zIndex = "0";
      }

      this.toolbox.draw();
      this.toolbox.parent.draw();
   }

   remove() {
      if (this.form) {
         this.form.div.style.zIndex = "0";
         this.form.div.contentEditable = false;
      }
      super.remove();
   }


}
