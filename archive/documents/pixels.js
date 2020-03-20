HTMLImageElement.prototype.loadImageData = function(src, scale) {
    
    if (scale == undefined)
        scale = 1;
    var canvas = document.createElement("canvas");

    if (window.localStorage[src + scale]) {
        var context = canvas.getContext("2d");
        alert(context);
        this.imageData = ImageData.createFromString(window.localStorage[src + scale]);
        if (this.onloadimagedata) {
            this.onloadimagedata();
        }
        return true;
    }
    this.addEventListener("load", onLoad, false);
    this.src = src;
    return false;

    function onLoad() {
        canvas.width = this.width*scale;
        canvas.height = this.height*scale;
        var context = canvas.getContext("2d");
        context.drawImage(this, 0, 0, canvas.width, canvas.height);
        this.imageData = context.getImageData(0, 0, canvas.width, canvas.height);
        window.localStorage[src + scale] = this.imageData.toString();
        if (this.onloadimagedata) {
            this.onloadimagedata();
        }
    }
}

function Pixels(imageData) {
    this.imageData = imageData;
    this.length = imageData.width * imageData.height;
}


Pixels.prototype = new Array();

Pixels.prototype.loadImageData = function() {
    var data = this.imageData.data;
    var i, pixel;
    for (i = 0, pixel = 0; i < data.length; i += 4, pixel++) {
        this[pixel] = i;
    }
}

Pixels.prototype.averageColor = function() {
    var data = this.imageData.data;
    var red = 0;
    var green = 0;
    var blue = 0;
    var i;
    for (var pixel = 0; pixel < this.length; pixel++) {
        i = this[pixel];
        red += data[i];
        green += data[i + 1];
        blue += data[i + 2];
    }
    red /=this.length;
    green /= this.length;
    blue /= this.length;
    return new RGB(red, green, blue);
}

CanvasRenderingContext2D.prototype.drawPixels = function(pixels, x, y, width, height) {
    if (x == undefined)
        x = 0;
    if (y == undefined)
        y = 0;

    if (width == undefined)
        width = pixels.imageData.width;
    if (height == undefined)
        height = pixels.imageData.height;
    var data = pixels.imageData.data;
    var imageData;
    if (data.length == pixels.length)
        imageData = pixels.imageData;
    else {
        imageData = context.createImageData(pixels.imageData.width, pixels.imageData.height);
        pixels.drawImageData(imageData);
    }
    this.putImageData(imageData, 0, 0, x, y, width, height);
}

Pixels.prototype.drawImageData = function(destinationImageData) {
    var source = this.imageData.data;
    var destination = destinationImageData.data;
    var i, pixel;
    for (pixel = 0; pixel < this.length; pixel++) {
        i = this[pixel];
        destination[i] = source[i];
        destination[i + 1] = source[i + 1];
        destination[i + 2] = source[i + 2];
        destination[i + 3] = 255;
    }
}

Pixels.prototype.convert = function(convertor) {
    var data = this.imageData.data;
    for (var pixel = 0; pixel < this.length; pixel++) {
         convertor(data, i);
    }
}

Pixels.prototype.paint = function(imageData, rgb) {
    var data = imageData.data;
    for (var pixel = 0; pixel < this.length; pixel++) {
        var i = this[pixel];
        data[i] = rgb.red;
        data[i+1] = rgb.green;
        data[i+2] = rgb.blue;
        data[i+3] = rgb.alpha;
    }
}

Pixels.prototype.averageCoordinates = function() {
    var x = 0;
    var y = 0;
    for (var pixel = 0; pixel < this.length; pixel++) {
        var coordinates = this.coordinates(pixel);
        x += coordinates.x;
        y += coordinates.y;
    }
    x /= this.length;
    y /= this.length;
    return { x : x, y : y };
}

Pixels.prototype.coordinates = function(pixel) {
    var i = this[pixel];
    var x = i % this.imageData.width;
    var y = (i - x) / this.imageData.width;
    return { x : x, y : y };
}
