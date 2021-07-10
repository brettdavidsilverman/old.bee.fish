class Toolbox extends Item {

   constructor(input) {
      super();
      
      this.children.push(
         new Pointer({object: new Delete()})
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