// Namespace Brett David Silverman
var bds;
if (bds == undefined) {
    bds = {};
}

//
// Make an element with [id] attribute moveable
// eg
// <div id="myDiv"></div>
// <script>
//     bds.makeMoveable("myDiv");
// </script>
bds.makeElementMoveable = function(id) {

    var target = document.getElementById(id);

    // Capture the start move event
    document.body.addEventListener("touchstart", moveStart);
    document.body.addEventListener("mousedown", moveStart);

        
    function moveStart(event) {
        // Capture the move event
        document.body.addEventListener("touchmove", move);
        document.body.addEventListener("mousemove", move);

        // Capture the end move  event
        document.body.addEventListener("touchend", moveEnd);
        document.body.addEventListener("mouseup", moveEnd);

        var coordinates = bds.getCoordinates(event);

        target.coordinates = coordinates;
    }

    // Handler for move event
    function move(event) {

        // Get the coordinates
        var coordinates = bds.getCoordinates(event);
        
        // Calculate the difference between this and the last position
        var delta = {};
        delta.x = target.coordinates.x  - coordinates.x;
        delta.y = target.coordinates.y - coordinates.y;

        // Set the new position
        target.style.left = (target.offsetLeft - delta.x) + "px";
        target.style.top = (target.offsetTop - delta.y) + "px";
        target.style.position = "absolute";

        // Save the last coordinates
        target.coordinates = coordinates;

        
    }


    // Handle the move end event
    function moveEnd(event) {
        // Capture the move event
        document.body.removeEventListener("touchmove", move);
        document.body.removeEventListener("mousemove", move);

        // Capture the end move  event
        document.body.removeEventListener("touchend", moveEnd);
        document.body.removeEventListener("mouseup", moveEnd);

    }


}

// Get the coordianges of a touch or mouse event
bds.getCoordinates = function(event) {

    var coordinates = {};
    if (event.touches) {
        // Touch event (use the first touch)
        var touch = event.touches[0];
        coordinates.x = touch.clientX;
        coordinates.y = touch.clientY;
        coordinates.target = touch.target;
    }
    else {
        // Mouse event
        coordinates.x = event.clientX;
        coordinates.y = event.clientY;
        coordinates.target = event.target;
    }
    
    return coordinates;

}