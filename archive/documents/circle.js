function Circle(centre, radius) {
    this.centre = centre;
    this.radius = radius;
}

Circle.prototype.toString = function() {
    return JSON.stringify(this);
}

Circle.fromThreePoints = function(point1, point2, point3) {
    var lineA = Line.fromTwoPoints(point1, point2);
    var lineB = Line.fromTwoPoints(point2, point3);
    var perpendicularLineA = Line.fromPointAndGradient(Point.average(point1, point2), lineA.perpendicularGradient());
    var perpendicularLineB = Line.fromPointAndGradient(Point.average(point2, point3), lineB.perpendicularGradient());
    var centre = Point.fromIntersection(perpendicularLineA, perpendicularLineB);
    var radius = centre.distanceTo(point1);
    return new Circle(centre, radius);
}