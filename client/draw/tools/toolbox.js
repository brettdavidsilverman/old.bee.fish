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

      var tools = [
         this.deleteTool,
         this.labelTool,
         this.valueTool,
         this.functionTool,
         this.flowTool
      ];

      tools.forEach(
         tool => children.push(tool)
      );

      this.dimensions = new Dimensions(
         {
            min: new Point(input.first.dimensions.min),
            max: new Point(input.last.dimensions.max)
         }
   
      );

      this.parent = this.canvas;
      
      this.canvas.children.push(this);

      this.matrix = new Matrix();
      var canvasTransform = this.canvas.transform;
      this.canvas.transform = async function(matrix) {
         self.transform(matrix);
         canvasTransform.call(self.canvas, matrix);
      }
   }

   async transform(matrix) {
      this.matrix.multiplySelf(matrix);
   }

   save() {

   }

}