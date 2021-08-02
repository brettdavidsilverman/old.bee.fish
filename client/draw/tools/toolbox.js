class Toolbox extends Canvas {

   parent;

   constructor(input) {
      super(input ? input.canvas : null);
      var self = this;

      if (input == undefined)
         input = {}

      this.parent = input.parent;

      input.toolbox = this;

      this.labelTool = new LabelTool(input);
      input.first = this.labelTool;
      input.last = this.labelTool;

      this.valueTool = new ValueTool(input);
      input.last = this.valueTool;

      this.functionTool = new FunctionTool(input);
      input.last = this.functionTool;

      this.flowTool = new FlowTool(input);
      input.last = this.flowTool;

      this.deleteTool = new DeleteTool(input);
      input.last = this.deleteTool;

      var children = this.children;

      this.tools = [
         this.deleteTool,
         this.labelTool,
         this.valueTool,
         this.functionTool,
         this.flowTool
      ];

      this.tools.forEach(
         tool => children.push(tool)
      );

      this.matrix = new Matrix();
   }

   save() {

   }

   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit = await this.hitTest(point);

<<<<<<< HEAD
      if (hit && hit.click instanceof Function)
          hit.click(point);
      else {

         this.parent.selection.selected = false;
         this.parent.selection = null;
            
         this.parent.draw();

         this.remove();


      }

   }
   
   async longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit = await this.hitTest(point);

      if (!hit) {

         this.parent.selection.selected = false;
         this.parent.selection = null;
         this.parent.draw();
            
         this.remove();

      }

   }

   toJSON() {
      return {
         canvas: super.toJSON()
      }
=======
      this.parent = this.canvas;

      this.canvas.children.push(this);

      window.stack.push(this.canvas);
   }

   toJSON() {
      return {
         item: super.toJSON(),
         canvas: this.canvas.toJSON()
      }
   }

   remove() {
      super.remove();
      this.canvas.remove();
   }
   
   async hitTest(point) {
      var hit = await this.children.hitTest(point);
      if (hit == null)
         this.remove();
      return hit;
>>>>>>> e9ee9814d12429ef1bc2f6c72b276d4463a79323
   }

}