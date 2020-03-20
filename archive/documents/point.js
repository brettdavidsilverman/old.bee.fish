function Point(touch, x, y) {
    this.x = x;
    this.y = y;
    this.touch = touch;
//    Point.publish(touch, this);
}
//new Publisher(Point);

Point.fromPageCoordinates = function(element, pageX, pageY) {
    var clientRect = element.getBoundingClientRect();
    var point = new Point(element.touch, pageX - clientRect.left - window.scrollX, pageY - clientRect.top - window.scrollY);
    return point;
}


Point.prototype.lineTo = function(toPoint) {
    var line = Line.fromTwoPoints(this, toPoint);
 }

Point.prototype.distanceTo = function(point) {
    var deltaX = point.x - this.x;
    var deltaY = point.y - this.y;
    return Math.sqrt(deltaX * deltaX + deltaY * deltaY);
}

Point.prototype.equals = function(point) {
    return (this.x == point.x && point.y == point.y);
}

Point.fromIntersection = function(line1, line2) {
    if (line1.gradient == line2.gradient)
        throw "Parellel lines do not intersect";

    var x = (line2.offset - line1.offset) / (line1.gradient - line2.gradient);
    var y = line1.y(x);
    return new Point(x, y);
}

Point.average = function(point1, point2) {
    var x = (point1.x + point2.x) / 2;
    var y = (point1.y + point2.y) / 2;
    return new Point(x, y);
}

Point.prototype.toString = function() {
    return JSON.stringify(this);
}
