class ToolboxItem extends Item {
   width = 100;
   height = 100;
   topLeft;
   canvas;

   constructor(input) {
      super({index : 0});

      if (input == undefined)
         input = {}
      
      if (input.last == undefined)
         this.topLeft = new Point({x: 10, y: 10});
      else {
         var width = input.canvas.width;
         var x = input.last.topLeft.x + input.last.width + 10;
         var y = input.last.topLeft.y;
         console.log("1");
         if ((x + this.width + 5) > width) {
            console.log("2");
            x = 10;
            y += input.last.height + 10;
         }

         this.topLeft = new Point({x, y});     
      }
      
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
      
      this.matrix = new Matrix();
      this.canvas = input.canvas;

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