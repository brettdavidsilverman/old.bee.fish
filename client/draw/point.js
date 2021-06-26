class Point extends DOMPoint {

   
   constructor(input) {
      super(input.x, input.y, input.z);
      input.name = this.constructor.name;
      var id = new Id(input);
      Object.assign(this, id);
   }

   toJSON() {
      return {
         ms: this.ms,
         inc: this.inc,
        // key: this.id.key,
         x: this.x,
         y: this.y,
         z: this.z
      }
   }
   
   toString()
   {
      return JSON.stringify(
         {
            x: this.x,
            y: this.y,
            z: this.z
         }
      );
   }
   
   matrixTransform(matrix)
   {
      var point =
         new Point(
            super.matrixTransform(matrix)
         );
      return point;
   }
   
   static fromPoint(source) {
      var copy = new Point(
         {
            time: source.time,
            increment: source.increment,
            x: source.x,
            y: source.y,
            z: source.z
         }
      );
      return copy;
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
      return Point.fromPoint(this);
   }
}
