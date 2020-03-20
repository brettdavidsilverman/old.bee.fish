function scribble(script) {
    var parentNode = script.parentNode;
    var canvas = document.createElement("canvas");
    canvas.width = 200;
    canvas.height = 200;

    parentNode.appendChild(canvas);
    canvas.context = canvas.getContext("2d");
    canvas.context.lineWidth = 1;
    canvas.context.strokeStyle = "black";
    canvas.ontouchstart = ontouchstart;
    canvas.ontouchmove = ontouchmove;
    canvas.ontouchend = ontouchend;
    canvas.points = [];
}
function ontouchstart(event) {
    var point = Point(this, event);

    event.preventDefault();
}

function ontouchmove(event) {
/*
    var point = Point(this, event);

    this.context.lineTo(point.x, point.y);
    this.context.stroke();
*/
}

function ontouchend(event) {
   if (this.points.length == 4) {
       var p1 = this.points[0];
       var p2 = this.points[1];
       var p3 = this.points[2];
       var p4 = this.points[3];

       var x = cx(p1, p2, p3, p4);
       var y = cy(x, p1, p2)
       this.context.moveTo(p2.x, p2.y);
       this.context.quadraticCurveTo(x, y, p3.x, p3.y);
       this.context.stroke();
       this.points.shift();
       //this.points[0] = p(x, y);
       //alert([this.points[0].x, this.points[0].y]);
   }
}

function Point(canvas, event) {
    var touch = event.touches[0];
    var date = new Date();
    var x = touch.clientX;
    var y = touch.clientY;
    var point = p(x, y);
    canvas.points.push(point);
    return point;
}

function p(x, y) {
    return { x : x, y : y };
}
function cx(p1, p2, p3, p4) {
    return (p3.y - p2.y + ((p2.y - p1.y) / (p2.x - p1.x)) * p2.x - ((p3.y - p4.y) / (p3.x - p4.x)) * p3.x) / ((p2.y - p1.y) / (p2.x - p1.x) - (p3.y - p4.y) / (p3.x - p4.x));
}

function cy(x, p1, p2) {
    return ((p2.y - p1.y) / (p2.x - p1.x)) * (x - p2.x)  + p2.y
}

