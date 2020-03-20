var Point = {}

Point.toString = function(point, precision) {

   if (!precision && precision != 0)
      precision = 2;
      
   var p = Point.round(point, precision);
   
   return JSON.stringify(p);
}
   
Point.min = function(point1, point2) {

   var min = {
      x: Math.min(point1.x, point2.x),
      y: Math.min(point1.y, point2.y)
   }
   
   return min;

}

Point.max = function(point1, point2) {

   var max = {
      x: Math.max(point1.x, point2.x),
               
      y: Math.max(point1.y, point2.y)
   }
   
   return max;
}

Point.equals = function(point1, point2) {
   return (point1.x == point2.x &&
           point1.y == point2.y);
}

Point.middle = function(point1, point2) {
   return {
      x: (point1.x + point2.x) / 2,
      y: (point1.y + point2.y) / 2
   }
}

Point.translate = function(point, translate) {
   return {
      x: (point.x + translate.x),
      y: (point.y + translate.y)
   }
}

Point.scale = function(point, scale) {

   return {
      x: point.x * scale.x,
      y: point.y * scale.y
   }
   
}

Point.negative = function(point) {

   return {
      x: -point.x,
      y: -point.y
   }
   
}

Point.reciprocal = function(point) {

   return {
      x: 1 / point.x,
      y: 1 / point.y
   }
   
}

Point.distance = function(p1, p2) {
   var dx = (p2.x - p1.x);
   var dy = (p2.y - p1.y);
   return Math.sqrt(dx * dx + dy * dy);
}

Point.screenToCanvas =
   function(canvas, point) {
  
   return
      canvas.matrix.transform(
         point
      );
}

Point.round = function(point, precision) {
   
   if (!precision)
      precision = 0;
         
   var x = Math.round(point.x * Math.pow(10, precision)) / Math.pow(10, precision);
   
   var y = Math.round(point.y * Math.pow(10, precision)) / Math.pow(10, precision);

   return {
      x: x,
      y: y
   }
}

Point.clone = function(source) {
   return {
      x: source.x,
      y: source.y,
      time: source.time
   }
}