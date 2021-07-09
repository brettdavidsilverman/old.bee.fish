class ToolboxItem extends Item {
   width = 100;
   height = 100;
   topLeft;
   
   constructor(input) {
      super(input);
      
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
   }
}