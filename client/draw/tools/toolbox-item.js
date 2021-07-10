class ToolboxItem extends Item {
   width = 100;
   height = 100;
   topLeft;
   
   constructor(input) {
      super(Object.assign(input, {index: 0}));
      
      this.topLeft = new Point(input.topLeft);
      
      this.dimensions =
         new Dimensions(
            {
               min: this.topLeft,
               max: new Point(
                  {
                     x: this.topLeft.x + this.width,
                     y: this.topLeft.y + this.height
                  }
               )
            }
         );
      
      this.dimensioned = true;
      this.matrix = new Matrix();

   }


   async draw(context) {
      
      this.pushMatrix(context);

      var rectangle = new Rectangle({item: this});
      rectangle.fillStyle = "rgba(256,256,0, 0.25)";
      await rectangle.draw(context);
      await super.draw(context);

      this.popMatrix(context);
   }

   async hitTest(point, matrix) {
   
      var hit =
         this.dimensions
         .isPointInside(point);
           
      if (hit)
      {
         return this;
      }
      
      return null;
   }
   
}