class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input ? input.item : null);
      var self = this;

      if (input.canvas)
         this.canvas = new Canvas(input);
      else
         this.canvas = new Canvas();

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

      // Calculate dimensions
      var dim = new Dimensions();
      this.tools.forEach(
         tool => {
            dim.min = Point.min(dim.min, tool.dimensions.min);
            dim.max = Point.max(dim.max, tool.dimensions.max);
         }
      );

      this.dimensions = dim;

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
   }
}