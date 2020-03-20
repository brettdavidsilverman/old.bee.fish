HTMLImageElement.prototype.getPixels = function(scale) {
    if (scale == undefined)
        scale = 1;
    var canvas = document.createElement("canvas");
    canvas.width = this.width*scale;
    canvas.height = this.height*scale;
    var context = canvas.getContext("2d");
    context.drawImage(this, 0, 0, canvas.width, canvas.height);
    var imageData = context.getImageData(0, 0, canvas.width, canvas.height);
    var pixels = Pixels.loadImageData(imageData);
    return pixels;
}

function Pixels() {
}
Pixels.prototype = new Array();

Pixels.loadImageData = function(imageData) {
    var pixels = new Pixels();
    pixels.length = imageData.width * imageData.height;
    var data = imageData.data;
    var i = 0;
    var pixel = 0;
    for (var x = 0; x < data.width; x++) {
        for (var y = 0; y < data.height; y++, pixel++) {
            pixels[pixel] = new Pixel(x, y, new Color(data[i], data[i + 1], data[i + 2]));
    }
}

Pixels.prototype.average = function() {
    var degrees = 0;
    var brightness = 0;
    var colorCount = 0;
    var x = 0;
    var y = 0;
    var pixel;
    for (var i = 0; i < pixels.length; i++) {
        pixel = this[i];
        x += pixel.x;
        y += pixel.y;
        if (pixel.degrees != undefined) {
            degrees += pixel.color.degrees;
            colorCount++;
        }
        brightness += pixel.color.brightness;
    }

    x /= pixels.length;
    y /= pixels.length;
    if (colorCount > 0)
        degrees /= colorCount;
    else
        degrees = undefined;
    brightness /= pixels.length;

    var color = new Color();
    color.degrees = degrees;
    color.brightness = brightness;
    return new Pixel(x, y, color);
}

Pixels.prototype.split = function (pixels) {
    var averageRed = 0;
    var averageGreen = 0;
    var averageBlue = 0;
    var data = pixels.imageData.data;
    var i;
    var pixel;
    for (pixel = 0; pixel < pixels.length; pixel++) {
        i = pixels[pixel];
        averageRed += data[i];
        averageGreen += data[i+1];
        averageBlue += data[i+2];
    }

    averageRed /= pixels.length;
    averageGreen /= pixels.length;
    averageBlue /= pixels.length;

    var highPixels = [];
    var lowPixels = [];
    
    for (pixel = 0; pixel < pixels.length; pixel++) {
        i = pixels[pixel];
        if (isHigh(averageRed, averageGreen, averageBlue, data[i], data[i+1], data[i+2]))
            highPixels.push(i);
        else
            lowPixels.push(i);
    }

    if (highPixels.length == 0)
        this.pixels = lowPixels
    else if (lowPixels.length == 0)
        this.pixels = highPixels;
    else {
        this.high = new splitImage(imageData, highPixels);
        this.low = new splitImage(imageData, lowPixels);
        }
    }

    splitImage.prototype.draw = function(high) {
        if (this.pixels != undefined) {
            var data = this.imageData.data;
            var pixels = this.pixels; 
            for (pixel = 0; pixel < pixels.length; pixel++) {
                i = pixels[pixel];
                if (high) {
                    //data[i] = 255;
                    data[i+1] = 0;
                    data[i+2] = 0;
                }
                else {
                    data[i] = 0;
                    data[i+1] = 0;
                //data[i+2] = 255;
                }
            }
        }
        else {
            this.high.draw(true);
            this.low.draw(false);
        }
    }

    function isHigh(averageRed, averageGreen, averageBlue, red, green, blue) {
        return (averageRed < red && averageGreen < green) || (averageGreen < green && averageBlue < blue) || (averageBlue < blue && averageRed < red);
    }

