class Toolbox extends Canvas {

   parent;

   constructor(input) {
      super(input ? input.canvas : null);
      var self = this;

      if (input == undefined)
         input = {}

      this.parent = input.parent;

      input.toolbox = this;

      this.deleteTool = new DeleteTool(input);
      input.first = this.deleteTool;
      input.last = this.deleteTool;

      this.labelTool = new LabelTool(input);
      input.last = this.labelTool;

      this.valueTool = new ValueTool(input);
      input.last = this.valueTool;

      this.functionTool = new FunctionTool(input);
      input.last = this.functionTool;

      this.flowTool = new FlowTool(input);
      input.last = this.flowTool;

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
   }

}