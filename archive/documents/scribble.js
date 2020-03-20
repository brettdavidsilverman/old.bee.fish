function scribble(script) {
    var parentNode = script.parentNode;
    var canvas = document.createElement("canvas");
    canvas.width = parentNode.clientWidth;
    canvas.height = parentNode.clientHeight;
    parentNode.appendChild(canvas);
    canvas.context = canvas.getContext("2d");
    canvas.context.lineWidth = 1;
    canvas.context.strokeStyle = "black";
    canvas.ontouchstart = ontouchstart;
    canvas.ontouchmove = ontouchmove;
    canvas.ontouchend = ontouchend;

}
function ontouchstart(event) {
    this.points = [];
    var point = Point(this, event);
    this.context.moveTo(point.x, point.y);
    event.preventDefault();
}

function ontouchmove(event) {
    var point = Point(this, event);

    this.context.lineTo(point.x, point.y);
    this.context.stroke();
 
}

function ontouchend(event) {
    alert(event.which);

}

function Point(canvas, event) {
    var touch = event.touches[0];
    var date = new Date();
    var x = touch.clientX;
    var y = touch.clientY;
    var point = { date : date, x : x, y : y };
    canvas.points.push(point);
    return point;
}
