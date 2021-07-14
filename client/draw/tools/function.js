class FunctionTool extends ToolboxItem {

   form;

   constructor(input) {
      super(input);

      this.label = "f(x)";
   }

   async draw(context) {

      await super.draw(context);

      context.pushMatrix(context);
      
      context.lineWidth = 1;
      context.fillStyle = "yellow";
      context.textAlign    = 'center';
      context.textBaseline = 'middle';
      context.font = "italic 40px Times New Roman";

      var dim = this.dimensions;

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);

      this.popMatrix(context);
   }

   async click(point) {
      var selection = this.canvas.selection;

      if (this.form == undefined) {
         // Create the form
         this.form = new Form(
            {
               canvas: this.canvas,
               item: selection
            }
         );
      }
      else {
         this.form.remove();
         this.form = null;
      }

      this.canvas.draw();
   }

}
