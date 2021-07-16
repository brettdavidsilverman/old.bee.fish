class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input ? input.item : null);

      this.canvas = input.canvas;

      this.deleteTool = new DeleteTool({toolboxItem : input});
      input.first = this.deleteTool;
      input.last = this.deleteTool;

      this.labelTool = new LabelTool({toolboxItem : input});
      input.last = this.labelTool;

      this.valueTool = new ValueTool({toolboxItem: input});
      input.last = this.valueTool;

      this.functionTool = new FunctionTool({toolboxItem: input});
      input.last = this.functionTool;

      this.flowTool = new FlowTool({toolboxItem: input});
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

      this.canvas.children.push(this);

      this.matrix = this.canvas.inverse.copy();

   }

   save() {

   }


}