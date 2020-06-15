// Load image

var app = this;
var image = new Image();
image.onload = function() {
   app.connectOutputs(app, image);
}
image.src = "/feebee.jpg";