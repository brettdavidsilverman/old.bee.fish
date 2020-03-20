function SplitPixels(pixels) {
    this.pixels = pixels;
    this.average = pixels.averageColor();
    var high = new Pixels(pixels.imageData);
    var low = new Pixels(pixels.imageData);
    var pixel;
    var lowCount = 0;
    var highCount = 0;
    var i;
    var data = pixels.imageData.data;
    for (var pixel = 0; pixel < pixels.length; pixel++) {
        i = pixels[pixel];
        var rgb = new RGB(data[i], data[i + 1], data[i + 2]);
        
        if (rgb.isLessThen(this.average))
            low[lowCount++] = i;
        else
            high[highCount++] = i;
    }
    low.length = lowCount;
    high.length = highCount;
    if (low.length != 0 && high.length != 0) {
        this.bottom = false;
        this.low = new SplitPixels(low);
        this.high = new SplitPixels(high);
    }
    else
        this.bottom = true;
}

SplitPixels.prototype.toString = function() {
}

function traceComplex(split, traceFunction) {
    if (this.bottom || split.bottom) {
        return traceFunction(this, split);
    }

    if (this.high.average.isLessThen(split.average))
        this.trace(split.low, traceFunction);
    else if (split.average.isLessThen(this.low.average))
        this.trace(split.high, traceFunction);
    else {
        this.low.trace(split.low,traceFunction);
        this.high.trace(split.high, traceFunction);
    }

}

function trace(split, traceFunction) {
    if (this.bottom || split.bottom) {
        return traceFunction(this, split);
    }

    this.low.trace(split.low,traceFunction);
    this.high.trace(split.high, traceFunction);

}

SplitPixels.prototype.trace = traceComplex;