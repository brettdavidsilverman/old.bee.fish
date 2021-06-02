class Point extends Id {
   x;
   y;
   
   constructor(input) {
      super(input);
      Object.assign(this, input);
   }

   toJSON() {
      var object = {
         time: this.time,
         increment: this.increment,
         x: this.x,
         y: this.y
      }
      return object;
   }
   
   static copy(source) {
      return new Point(source);
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
      return new Point(
         {
            x: Math.max(
               p1.x,
               p2.x
            ),
            y: Math.max(
               p1.y,
               p2.y
            )
        }
      );
   }

   static min(p1, p2) {
      return new Point(
         {
            x: Math.min(
               p1.x,
               p2.x
            ),
            y: Math.min(
               p1.y,
               p2.y
            )
         }
      );
   }
   
   copy() {
      return new Point(this);
   }
}
