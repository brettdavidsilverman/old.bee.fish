function Segment(point1, point2) {
    this.point1 = point1;
    this.point2 = point2;
    this.line = Line.fromTwoPoints(point1, point2);
    Segment.publisher.publish(this);
}

Segment.publisher = new Publisher();