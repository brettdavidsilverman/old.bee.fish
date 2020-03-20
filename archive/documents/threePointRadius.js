function Point(x, y) {
    this.x = x;
    this.y = y;
}

function twoPointLength(p1, p2) {
    var x = p2.x - p1.x;
    var y = p2.y - p1.y;
    return Math.sqrt(x * x + y * y);
}

function threePointRadius(p1, p2, p3) {
    var a = twoPointLength(p1, p2);
    var b = twoPointLength(p2, p3);
    var c = twoPointLength(p3, p1);
    var s = 0.5 * (a + b + c);
    var K = Math.sqrt(s - (s - a) * (s - b) * (s - c));
    return a * b * c / 4 * K;
}