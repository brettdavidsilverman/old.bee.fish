function Canvas(parentElement) {
    this.parentElement = parentElement;
    this.canvas = document.createElement("canvas");
    Canvas.canvases.push(this);
    parentElement.appendChild(this.canvas);
    this.touch = new Touch(this.canvas);
    this.touch.canvas = this;
    this.canvas.getContext("2d").fillStyle = "black";
    TouchStart.subscribe(this.touch, Canvas.touchStart);
    TouchMove.subscribe(this.touch, Canvas.touchMove);
    TouchEnd.subscribe(this.touch, Canvas.touchEnd);
    Line.subscribe(this.touch, Canvas.line);
    Point.subscribe(this.touch, Canvas.point);
    this.resize();
}

Canvas.canvases = [];

Canvas.point = function(point) {
    var canvas = this.canvas.canvas;
    var context = canvas.getContext("2d");
    context.fillStyle = "black";
    context.fillRect(point.x, point.y, 1, 1);
}

Canvas.line = function(line) {
    var canvas = this.canvas.canvas;
    var context = canvas.getContext("2d");
    context.lineStyle = "black";
    context.beginPath();
    context.moveTo(line.fromPoint.x, line.fromPoint.y);
    context.lineTo(line.toPoint.x, line.toPoint.y);
    context.stroke();
}

Canvas.touchStart = function(touchStart) {
    touchStart.points = [];
    touchStart.points.push(touchStart.point);
}

Canvas.touchMove = function(touchMove) {
    touchMove.touchStart.points.push(touchMove.toPoint);
}

Canvas.touchEnd = function(touchEnd) {
    var points = touchEnd.touchStart.points;
    this.canvas.paint();
    var canvas = this.canvas.canvas;
    var context = canvas.getContext("2d");
    context.lineWidth = 1;
    context.lineStyle = "red";
    context.beginPath();
    context.moveTo(points[0].x, points[0].y);
    var i;
    for (var i = 1; i < points.length - 2; i ++) {
      var xc = (points[i].x + points[i + 1].x) / 2;
      var yc = (points[i].y + points[i + 1].y) / 2;
      context.quadraticCurveTo(points[i].x, points[i].y, xc, yc);
    }
 // curve through the last two points
    if (i < points.length - 2)
        context.quadraticCurveTo(points[i].x, points[i].y, points[i+1].x,points[i+1].y);
    context.stroke();
}
Canvas.onResize = function(event) {
    for (var index = 0; index < Canvas.canvases.length; index++) {
        var canvas = Canvas.canvases[index];
        canvas.resize();
    }
}


window.addEventListener("resize", Canvas.onResize, false);

Canvas.prototype.resize = function() {
    var newWidth = this.parentElement.clientWidth;
    var newHeight = this.parentElement.clientHeight;
    if (this.canvas.width == newWidth && this.canvas.height == newHeight)
        return;

    this.canvas.width = newWidth;
    this.canvas.height = newHeight;
    this.paint();
}

Canvas.prototype.paint = function() {
    var context = this.canvas.getContext("2d");
    context.strokeStyle = "blue";
    context.fillStyle = "white";
    context.fillRect(0, 0, this.canvas.width, this.canvas.height);
    context.strokeRect(0, 0, this.canvas.width, this.canvas.height);
    context.stroke();
}

