class FlowTool extends ToolboxItem {

   _running;

   constructor(input) {
      super(
         Object.assign(
            input,
            {
               topLeft: {x: 340, y: 10},
            }
         )
      );

      this.label = "⇣";

   }

   async draw(context) {

      await super.draw(context);

      this.pushMatrix(context);

      var dim = this.dimensions;

      if (this.running) {
         context.fillStyle = "rgba(0, 0, 255, 0.6)";
         context.fillRect(dim.min.x, dim.min.y, dim.width, dim.height);
      }

      context.lineWidth = 1;
      context.fillStyle = "black";
      context.textAlign    = 'center';
      context.textBaseline = 'middle';
      context.font = "40px Courier new";

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);

      this.popMatrix(context);
   }

   async click(point) {
      
      var flowTool = this;
      
      if (this.running) {
         this.running = false;
         this.canvas.draw();
         return;
      }

      var selection = this.canvas.selection;
      if (!confirm("Select next item to flow to"))
         return;

      this.running = true;
      var saveClick = this.canvas.click;

      this.canvas.click = async function(point) {

         var hit  = await this.children.hitTest(
            point,
            this.matrix
         );

         if (hit && this.selection && hit != this.selection)
         {
            join(this.selection, hit);
         }

         this.click = saveClick;
         flowTool.running = false;
         
         this.draw();

         function join(fromItem, toItem) {
            alert([fromItem.label, toItem.label]);
         }
      }

      this.canvas.draw();
   }

   get running() {
      return this._running;
   }

   set running(value) {
      this._running = value;
      var toolbox = this.canvas.toolbox;
      toolbox.deleteTool.visible = !this.running;
      toolbox.labelTool.visible = !this.running;
      toolbox.valueTool.visible = !this.running;
   }
}
