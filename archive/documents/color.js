function Color(degrees, brightness) {
    this.degrees = degrees;
    this.brightness = brightness;
}

function RGB(red, green, blue, alpha) {
    this.red = red;
    this.green = green;
    this.blue = blue;
    this.alpha = alpha;
}

RGB.prototype.isLessThen = function(rgb) {
    return (this.toColor().isLessThen(rgb.toColor()));
    return ((this.red < rgb.red && this.green < rgb.green)
        || (this.green < rgb.green && this.blue < rgb.blue)
        || (this.blue < rgb.blue && this.red < rgb.red));
}

RGB.prototype.toString = function() {
    return JSON.stringify(this);
}


RGB.prototype.toColor = function() {
    var degrees;
    var brightness = (this.red + this.green + this.blue) / 3 / 255;
    if (this.red == this.green && this.green == this.blue && this.blue == this.red)
        degrees = undefined;
    else {
        var colors;
        if (this.red >= this.green && this.red >= this.blue) {
            degrees = 0;
            colors = [this.blue, this.red, this.green];
        }
        else if (this.green >= this.blue && this.green >= this.red) {
            degrees = 120;
            colors = [this.red, this.green, this.blue];
        }
        else {
            degrees = 240;
            colors = [this.green, this.blue, this.red];
        }
        colors[0] = -(120 * (colors[0] + colors[1]) / 2 / 255);
        colors[2] = (120 * (colors[1] + colors[2]) / 2 / 255);
        degrees = degrees + (colors[0] + colors[2]) / 2;
        if (degrees < 0)
            degrees += 360;
    }
    var color = new Color(degrees, brightness);
    //color.rgb = this;
    return color;
}

Color.prototype.isLessThen = function(color) {

    if (this.degrees == undefined || color.degrees == undefined)
        return this.brightness < color.brightness;

    if (this.degrees < 180 && color.degrees < 180)
        return this.degrees < color.degrees;

    if (this.degrees < 180 && color.degrees >= 180)
        return (this.degrees + 180) < color.degrees;
 
    if (this.degrees >= 180 && color.degrees < 180)
        return (this.degrees - 180) < color.degrees;

    // both >= 180
    return this.degrees < color.degrees;
}

Color.prototype.toString = function() {
    return JSON.stringify(this);
}

Color.prototype.toRGB = function() {
    if (this.degrees == undefined) {
        var grey = (this.brightness * 255);
        return new RGB(grey, grey, grey);
    }

    var red, green, blue;
    if (this.degrees < 120) {
        red = this.degrees;
        green = 120 - this.degrees;
        if (this.degrees <= 60)
            blue = 120 + this.degrees;
        else
            blue = 240 - this.degrees;
    }
    else if (this.degrees < 240) {
        if (this.degrees <= 180)
            red = this.degrees;
        else
            red = 360 - this.degrees;
        green = this.degrees - 120;
        blue = 240 - this.degrees;
    }
    else {
        red = 360 - this.degrees;
        if (this.degrees <= 300)
            green = this.degrees - 120;
        else
            green = 360 - this.degrees + 120;
        blue = this.degrees - 240;
    }

    return new RGB(red, green, blue);

    var redPortion = 1 - red/120;
    var greenPortion = 1 - green/120;
    var bluePortion = 1 - blue/120;

    return new RGB(redPortion, greenPortion, bluePortion);

   // max = this.brightness * 255 * 3 * (1 - (small1 + small2));// (redPortion/(redPortion + greenPortion + bluePortion));
    green = this.brightness * 255 * (1 - (redPortion + bluePortion));
    blue = this.brightness * 255 * (1 - (greenPortion + redPortion));

    return new RGB(red, green, blue);

}

var rgb = new RGB(125, 1, 0);
var color = rgb.toColor();
var rgb2 = color.toRGB();
var color2 = rgb2.toColor();
alert([rgb, color, rgb2, color2]);
/**
 * Converts an RGB color value to HSL. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes r, g, and b are contained in the set [0, 255] and
 * returns h, s, and l in the set [0, 1].
 *
 * @param   Number  r       The red color value
 * @param   Number  g       The green color value
 * @param   Number  b       The blue color value
 * @return  Array           The HSL representation
 */
function rgbToHsl(r, g, b){
    r /= 255, g /= 255, b /= 255;
    var max = Math.max(r, g, b), min = Math.min(r, g, b);
    var h, s, l = (max + min) / 2;

    if(max == min){
        h = s = 0; // achromatic
    }else{
        var d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
        switch(max){
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
    }

    return [h, s, l];
}

/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   Number  h       The hue
 * @param   Number  s       The saturation
 * @param   Number  l       The lightness
 * @return  Array           The RGB representation
 */
function hslToRgb(h, s, l){
    var r, g, b;

    if(s == 0){
        r = g = b = l; // achromatic
    }else{
        function hue2rgb(p, q, t){
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1/6) return p + (q - p) * 6 * t;
            if(t < 1/2) return q;
            if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
        }

        var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        var p = 2 * l - q;
        r = hue2rgb(p, q, h + 1/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1/3);
    }

    return [r * 255, g * 255, b * 255];
}

/**
 * Converts an RGB color value to HSV. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
 * Assumes r, g, and b are contained in the set [0, 255] and
 * returns h, s, and v in the set [0, 1].
 *
 * @param   Number  r       The red color value
 * @param   Number  g       The green color value
 * @param   Number  b       The blue color value
 * @return  Array           The HSV representation
 */
function rgbToHsv(r, g, b){
    r = r/255, g = g/255, b = b/255;
    var max = Math.max(r, g, b), min = Math.min(r, g, b);
    var h, s, v = max;

    var d = max - min;
    s = max == 0 ? 0 : d / max;

    if(max == min){
        h = 0; // achromatic
    }else{
        switch(max){
            case r: h = (g - b) / d + (g < b ? 6 : 0); break;
            case g: h = (b - r) / d + 2; break;
            case b: h = (r - g) / d + 4; break;
        }
        h /= 6;
    }

    return [h, s, v];
}

/**
 * Converts an HSV color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
 * Assumes h, s, and v are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   Number  h       The hue
 * @param   Number  s       The saturation
 * @param   Number  v       The value
 * @return  Array           The RGB representation
 */
function hsvToRgb(h, s, v){
    var r, g, b;

    var i = Math.floor(h * 6);
    var f = h * 6 - i;
    var p = v * (1 - s);
    var q = v * (1 - f * s);
    var t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return [r * 255, g * 255, b * 255];
}