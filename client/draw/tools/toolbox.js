class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input);
      var self = this;

      this.canvas = input.canvas;

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

      this.matrix = new Matrix();
   }

   async transform(matrix) {
      this.matrix.multiplySelf(matrix);
   }

   save() {

   }

   remove() {
      super.remove();
      this.canvas.toolbox = null;
   }
   
   async hitTest(point) {

      point = this.canvas.canvasToScreen(point);

      point = point.matrixTransform(this.matrix.inverse());

      var hit = this.children.hitTest(point);

      if (hit)
         return hit;

      return null;
   }  

   async draw(context) {
      context.pushMatrix(this.matrix);
      await super.draw(context);
      context.popMatrix();
   }
}