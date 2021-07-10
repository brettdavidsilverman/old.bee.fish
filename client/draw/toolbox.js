class Toolbox extends Item {

   canvas;

   constructor(input) {
      super(input);

      this.canvas = input.canvas;

      this.children.push(
         new Pointer({object: new Delete(input)})
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