class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input);

      this.canvas = input.canvas;

      const tools = [
         new Delete(input),
         new Label(input),
         new Value(input)
      ]

      var children = this.children;

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


}