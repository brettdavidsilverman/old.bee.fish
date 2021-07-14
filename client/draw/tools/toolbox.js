class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input);

      this.canvas = input.canvas;

      this.deleteTool = new DeleteTool(input);
      input.last = this.deleteTool;

      this.labelTool = new LabelTool(input);
      input.last = this.labelTool;

      this.valueTool = new ValueTool(input);
      input.last = this.valueTool;

      this.functionTool = new FunctionTool(input);
      input.last = this.functionTool;

      this.flowTool = new FlowTool(input);

      var children = this.children;

      var tools = [
         this.deleteTool,
         this.labelTool,
         this.valueTool,
         this.functionTool,
         this.flowTool
      ];

      tools.forEach(
         tool => children.push(new Pointer({object: tool}))
      );
   }

   async hitTest(point) {
      var matrix = new Matrix();
      var child =
         await this.children.hitTest(
            point, matrix
         );
      
      if (child)
         return child;

      return null;
   }

   save() {

   }

}