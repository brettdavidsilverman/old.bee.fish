function Line(touch, gradient, offset, fromPoint, toPoint) {
    this.gradient = gradient;
    this.offset = offset;
    this.fromPoint = fromPoint;
    this.toPoint = toPoint;
    this.touch = touch;
    Line.publish(touch, this);
}

new Publisher(Line);

Line.prototype.toString = function() {
    return JSON.stringify(this);
}

Line.prototype.y = function(x) {
    return this.gradient * x + this.offset;
}

Line.prototype.x = function(y) {
    return (y - this.offset) / this.gradient;
}

Line.prototype.perpendicularGradient = function() {
    return -1 / this.gradient;
}

Line.fromTwoPoints = function(fromPoint, toPoint) {
    var rise = (toPoint.y - fromPoint.y);
    var run = (toPoint.x - fromPoint.x);
    var gradient = rise / run;
    var line = Line.fromPointAndGradient(gradient, fromPoint, toPoint);
    return line;
}

Line.fromPointAndGradient = function(gradient, fromPoint, toPoint) {
    var offset = fromPoint.y - gradient * fromPoint.x;
    return new Line(toPoint.touch, gradient, offset, fromPoint, toPoint);
}
