class Delete extends ToolboxItem {

   canvas;

   constructor(input) {
      super(
         {
            topLeft: {x: 10, y: 10},
         }
      );

      this.canvas = input.canvas;
      

   }

   async draw(context) {

      await super.draw(context);

      context.pushStack(new Matrix());
      context.lineWidth = 1;
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
      
      context.popStack();
   }

   async click(point) {
      this.canvas.selection.remove();
      this.canvas.selection = null;
      this.canvas.draw();
   }
}
