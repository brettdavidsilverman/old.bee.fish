function diagram(element) {

    var func1 = new Function("func1");
    var func2 = new Function("func2");
    var func1_1 = new Function("func1_1");
    var func1_2 = new Function("func1_2");
    func1.subdiagrams.push(func1_1);
    func1.subdiagrams.push(func1_2);
    var diagram = new Diagram([func1, func2]);
    diagram.create(element);
    window.diagram = diagram;
    diagram.diagram();
    diagram.context().fillStyle = "#FFFFFF";
//alert([diagram.scale(), diagram.subdiagrams.scale(), func1.scale(), func1.subdiagrams.scale(), func1_1.scale()]);
    d();

}

function d() {
    var diagram = window.diagram;
    if (diagram.hit != undefined) {
        var hit = window.diagram.hit;
        var context = hit.context();
        var moveCoords = hit.coordinates();
        var moveCoordinates = hit.popCoordinates();
        var lastCoords = hit.coordinates();
        if (moveCoords.x != lastCoords.x ||
            moveCoords.y != lastCoords.y) {
            context.lineWidth = 2;
            context.strokeStyle = "#0000FF";
            hit.diagram();

            context.strokeStyle = "#000000";
            context.lineWidth = 1
            hit.pushCoordinates(moveCoordinates);
            hit.diagram();
            hit.lastCoordinates = hit.coordinates();
        }
        else {
            hit.pushCoordinates(moveCoordinates);
        }
    }
    //window.diagram.diagram();
    window.setTimeout(d, 1);
}

/*
var touchMove = false;

window.diagram =diagram;
diagram.canvas = Diagram.canvas;

diagram.canvas.addEventListener('touchstart',
    function(event) {
        touchMove = true;
        event.preventDefault();
    }
    , false
);

diagram.canvas.addEventListener('touchmove',
    function(event) {
        if (touchMove) {
if (event.touches.length > 1)
    alert(event.touches.length);
            var touch =  event.touches[0];
            x = touch.clientX - func1.radius() * func1.parentScale();
            y = touch.clientY - func1.radius() * func1.parentScale();
            event.preventDefault();
        }
    }
    , false
);

diagram.canvas.addEventListener('touchend',
    function(event) {
        touchMove = false;
        event.preventDefault();
    }
    , false
);


//alert(func1.parent.x());
var x =  0;// / func1.parentScale() ;
var y=  0;
func1.x = function() {
    var retval = x / func1.parentScale() - func1.parent.x() / func1.parentScale()
    return retval;
}

func1.y = function() {
    var retval = y / func1.parentScale() - func1.parent.y() / func1.parentScale()
    return retval;
}
*/
//diagram.canvas();
/*
diagram.canvas.onmousedown = function(event) {


    diagram.canvas.onmousemove = function(touch) {
        x = touch.clientX - func1.radius() * func1.parentScale();
        y = touch.clientY - func1.radius() * func1.parentScale();
        func1.onDiagram(func1.context());
    }
}
diagram.canvas.onmouseup = function(event) {
    Diagram.canvas.onmousemove = undefined;
}
diagram.canvas.onfocus = function() {
    alert("focus");
}
diagram.canvas.focus();
*/
