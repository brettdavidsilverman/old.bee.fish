function Point(input) {
   Object.assign(this, input);
   this.id;
}

Point.copy = function(source) {
   return new Point(source);
}

Point.distance = function(p1, p2) {
   var dx = p2.x - p1.x;
   var dy = p2.y - p1.y;
   
   return Math.sqrt(
      dx * dx + dy * dy
   );
}

Point.middle = function(p1, p2) {
   return new Point(
      {
         x: (p1.x + p2.x) / 2,
         y: (p1.y + p2.y) / 2
      }
   );
}

Point.max = function(p1, p2) {
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

Point.min = function(p1, p2) {
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
