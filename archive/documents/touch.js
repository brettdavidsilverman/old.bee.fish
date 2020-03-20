function Touch(element) {
    element.addEventListener("touchstart", Touch.onTouchStart, true);
    element.addEventListener("touchmove", Touch.onTouchMove, true);
    element.addEventListener("touchend", Touch.onTouchEnd, true);
    this.touching = [];
    this.touchEvent = new Publisher(this);
    element.touch = this;
}

//new Publisher(Touch);

function TouchStart(point) {
    this.point = point;
//    TouchStart.publish(point.touch, this);
}

//new Publisher(TouchStart);

function TouchMove(touchStart, fromPoint, toPoint) {
    this.fromPoint = fromPoint;
    this.toPoint = toPoint;
    this.touchStart = touchStart;
    this.line = fromPoint.lineTo(toPoint);
//    TouchMove.publish(toPoint.touch, this);
}

//new Publisher(TouchMove);

function TouchEnd(touchStart, point) {
    this.touchStart = touchStart;
    this.point = point;
//    TouchEnd.publish(point.touch, this);
}
//new Publisher(TouchEnd);

Touch.onTouchStart = function(event) {
alert(event);
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changedTouch = event.changedTouches[index];
//        var point = Point.fromPageCoordinates(event.target, changedTouch.pageX, changedTouch.pageY);
        var touch = new this.Touch(changedTouch.pageX, changedTouch.pageY);
        publish(touch);
        var touchStart = new TouchStart(point);
        this.touching[changedTouch.identifier] = { touchStart : touchStart, point : point };
    }
    event.preventDefault();
}

Touch.onTouchMove = function(event) {
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changedTouch = event.changedTouches[index];
        var touch = this.touching[changedTouch.identifier];
        var toPoint = Point.fromPageCoordinates(event.target, changedTouch.pageX, changedTouch.pageY);
        var touchMove = new TouchMove(touch.touchStart, touch.point, toPoint);
        this.touching[changedTouch.identifier].point = toPoint;
    }
    event.preventDefault();
}

Touch.onTouchEnd = function(event) {
    for (var index = 0; index < event.changedTouches.length; index++) {
        var changedTouch = event.changedTouches[index];
        var touch = this.touching[changedTouch.identifier];
        var touchEnd = new TouchEnd(touch.touchStart, touch.point);
        delete this.touching[changedTouch.identifier];
   }
   event.preventDefault();

}
