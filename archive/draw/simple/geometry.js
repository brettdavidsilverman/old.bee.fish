class Point extends Storable {
   constructor(
      input = {
         x: 0,
         y: 0
      }
   ) {
      super(input);
   }
   
   static distance(p1, p2) {
      var dx = p2.x - p1.x;
      var dy = p2.y - p1.y;
      return Math.sqrt(
         dx * dx + dy * dy
      );
   }

   static middle(p1, p2) {
      return new Point(
         {
            x: (p1.x + p2.x) / 2,
            y: (p1.y + p2.y) / 2
         }
      );
   }
   
   static max(p1, p2) {
      return new Point( {
         x: Math.max(
            p1.x,
            p2.x
         ),
         y: Math.max(
            p1.y,
            p2.y
         )
      } );
   }
   
   static min(p1, p2) {
      return new Point( {
         x: Math.min(
            p1.x,
            p2.x
         ),
         y: Math.min(
            p1.y,
            p2.y
         )
      } );
   }
   
   copy() {
      return new Point(this);
   }
   
   log(label) {
      console.log(
         label + ": " +
         "(" + this.x + "," +
               this.y + ")"
      );
   }
   
   
}

class Line extends Storable {
   
   
   constructor (
      input = {
         points: [],
         strokeStyle: undefined,
         lineWidth: undefined
      }
   )
   {
      super(input);

      if (!this.points)
         this.points = [];

      if (!this.strokeStyle)
         this.strokeStyle = "blue";
         
      if (!this.lineWidth)
         this.lineWidth = 1.0;
   
   }
   
   draw(context) {
      
      context.save();
      
      var points = this.points;
      context.strokeStyle = this.strokeStyle;
      context.lineWidth =
         this.lineWidth / 
         context.getScale().x;
      context.beginPath();
      
      if (points.length == 1) {
      
         context.arc(
            points[0].x,
            points[0].y,
            context.lineWidth / 2,
            0,
            Math.PI * 2
         );
         context.fill();
         return;
      }
      
      context.moveTo(
         points[0].x,
         points[0].y
      );

      points.slice(1).forEach(
         point =>
            context.lineTo(
               point.x,
               point.y
            )
      );
      
      context.stroke();

      context.restore();
   }
   
   transformScreenToCanvas() {
   
      this.points.forEach(
         point =>
            canvas
               .transformScreenToCanvas(
                  point
               )
      );
      

   }
   
   
   setDimensions() {
      var min = this.points[0];
      var max = this.points[0];
      this.points.forEach(
         point => {
            min = Point.min(min, point);
            max = Point.max(max, point);
         }
      );
      this.min = min;
      this.max = max;
   }
   

}

class Rectangle extends Line {

   constructor(
      input = {
         topLeft: new Point( {
            x: 0,
            y: 0
         } ),
         lineWidth: 0.5,
         strokeStyle: "yellow",
         fillStyle: "rgba(128,128,0, 0.25)",
         width: 10,
         height: 10
      }
   ) 
   {
      super(input);

      if (!this.strokeStyle)
         this.strokeStyle = "blaxk";
      
      if (!this.fillStyle)
         this.fillStyle = "blue";
 
      if (!this.lineWidth)
         this.lineWidth = 0.5;
 
      if (!this.topLeft)
         this.topLeft = new Point(
            {
               x: 0,
               y: 0
            }
         );
         
   }
   
   draw(context) {
      
      context.fillStyle = this.fillStyle;
      super.draw(context);
      context.fill();
   }
   
   resize() {
      this.points = [
         // start, top left
         this.topLeft,
         // top right
         new Point(
            {
               x: this.topLeft.x + 
                  this.width,
               y: this.topLeft.y
            } 
         ),
         // bottom right
         new Point(
            {
               x: this.topLeft.x +
                  this.width,
               y: this.topLeft.y -
                  this.height
            }
         ),
         // bottom left
         new Point(
            {
               x: this.topLeft.x,
               y: this.topLeft.y -
                  this.height
            }
         ),
         // top left
         this.topLeft
      ];
      this.setDimensions();

   }
   
   setDimensions() {
      this.min = new Point(
         {
            x: this.topLeft.x,
            y: this.topLeft.y -
               this.height
         }
      );
      this.max = new Point(
         {
            x: this.topLeft.x +
               this.width,
            y: this.topLeft.y
 
         }
      );
  
   }
   
   isPointInside(point) {
      var min = this.min;
      var max = this.max;
      if ( point.x >= min.x &&
           point.x <= max.x &&
           point.y >= min.y &&
           point.y <= max.y )
         return true;
      else
         return false;
   }
}


   
   
