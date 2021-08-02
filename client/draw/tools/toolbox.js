class Toolbox extends Canvas {

   parent;

   constructor(input) {
      super(input ? input.canvas : null);
      var self = this;

      if (input == undefined)
         input = {}

      this.parent = input.parent;

      input.canvas = this;

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

   
   async hitTest(point) {

      point = this.screenToCanvas(point);

      var hit = await this.children.hitTest(point);

      if (hit)
         return hit;

      this.remove();

      this.parent.selection.selected = false;
      this.parent.selection = null;

      this.parent.draw();
      
      return null;
   }
   
   toJSON() {
      return {
         canvas: super.toJSON()
      }
   }

}