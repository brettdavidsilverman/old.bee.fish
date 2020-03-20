function Touch(parentElement) {
    this.parentElement = parentElement;
    parentElement.style.height = "100%";
    parentElement.style.width = "100%";
    parentElement.style.margin = "0px";
    parentElement.style.overflow = "none";
    this.canvas = Touch.createCanvas();
    parentElement.appendChild(this.canvas);
    this.canvas.getContext("2d").fillStyle = "black";
    this.canvas.touch = this;
    this.Point = function(x, y) {
        this.x = x;
        this.y = y;
        var context = this.canvas.getContext("2d");
        context.fillStyle = "black";
        context.fillRect(this.x, this.y, 1, 1);
    }
    this.Point.prototype = this;

    Touch.canvases.push(this);
    this.resize();
}

Touch.createCanvas = function() {
    var canvas = document.createElement("canvas");
    canvas.addEventListener("touchstart", Touch.ontouchstart, true);
    canvas.addEventListener("touchmove", Touch.ontouchmove, true);
    canvas.addEventListener("touchend", Touch.ontouchend, true);
    canvas.touched = [];
    canvas.touching = [];
    return canvas;

}

Touch.resize = function(event) {
    for (var index = 0; index < Touch.canvases.length; index++) {
        var touch = Touch.canvases[index];
        touch.resize();
    }
}

window.addEventListener("resize", Touch.resize, false);

Touch.start = function(canvas, identifier, point) {
    var context = canvas.getContext("2d");
    context.fillStyle = "black";
    context.fillRect(point.x, point.y, 1, 1);

    this.identifier = identifier;
    this.points = [];
    this.points.push(point);
    canvas.touching[identifier] = this;
    canvas.touched.push(this);
    var context = canvas.getContext("2d");
    context.beginPath();
    context.moveTo(point.x, point.y);
}

Touch.move = function(canvas, identifier, point) {
    var touch = canvas.touching[identifier];
    var lastPoint = touch.points[touch.points.length - 1];
    touch.points.push(point);
    var context = canvas.getContext("2d");
    context.strokeStyle = "blue";
    context.beginPath();
    context.moveTo(lastPoint.x, lastPoint.y);
    context.lineTo(point.x, point.y);
    context.stroke();
    if (touch.points.length >= 3) {
        var last = touch.points.length - 1;
        var point1 = touch.points[last - 2];
        var point2 = touch.points[last - 1];
        var point3 = touch.points[last];
        if (!point1.equals(point2) && !point2.equals(point3) && !point3.equals(point1)) {
            var line1 = Line.fromTwoPoints(point1, point2);
            var line2 = Line.fromTwoPoints(point2, point3);
            if (line1.gradient != line2.gradient) {
                var circle = Circle.fromThreePoints(point1, point2, point3);
                context.fillStyle = "black";
                context.fillRect(circle.centre.x, circle.centre.y, 1, 1);
/*
               if (touch.lastDifferential == undefined)
                   touch.lastDifferential = circle.centre;
               else {
                   context.strokeStyle = "red";
                   context.beginPath();
                   context.moveTo(touch.lastDifferential.x, touch.lastDifferential.y);
                   context.lineTo(circle.centre.x, circle.centre.y);
                   touch.lastDifferential = circle.centre;
                   context.stroke();
               }
*/
            }
        }
    }
}

Touch.end = function(canvas, identifier, point) {
    var touch = canvas.touching[identifier];
    delete canvas.touching[identifier];
}

Touch.prototype.paint = function() {
    var context = this.canvas.getContext("2d");
    context.strokeStyle = "blue";
    context.fillStyle = "white";
    context.fillRect(0, 0, this.canvas.width, this.canvas.height);
    context.strokeRect(0, 0, this.canvas.width, this.canvas.height);
    context.stroke();
}

Touch.prototype.resize = function() {
    var newWidth = this.parentElement.clientWidth;
    var newHeight = this.parentElement.clientHeight;
    if (this.canvas.width == newWidth && this.canvas.height == newHeight)
        return;

    this.canvas.width = newWidth;
    this.canvas.height = newHeight;
    this.paint();
}

Touch.ontouchstart = function(event) {
    var touch = this.touch;
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changed = event.changedTouches[index];
        var clientRect = changed.target.getBoundingClientRect();
        var point = new touch.Point(changed.pageX - clientRect.left - window.scrollX, changed.pageY - clientRect.top - window.scrollY);
        //var touch = new Touch.start(this, changed.identifier, point);
    }
    event.preventDefault();

}

Touch.ontouchmove = function(event) {
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changed = event.changedTouches[index];
        var clientRect = changed.target.getBoundingClientRect();
        var point = new Point(changed.pageX - clientRect.left - window.scrollX, changed.pageY - clientRect.top - window.scrollY);
        //Touch.move(this, changed.identifier, point);
    }
    event.preventDefault();

}

Touch.ontouchend = function(event) {
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changed = event.changedTouches[index];
        var clientRect = changed.target.getBoundingClientRect();
        var point = new Point(changed.pageX - clientRect.left - window.scrollX, changed.pageY - clientRect.top - window.scrollY);
        //Touch.end(this, changed.identifier, point);
    }
    event.preventDefault();

}


Touch.canvases = [];


