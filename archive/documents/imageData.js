ImageData.create = function(width, height) {
    var canvas = document.getElementById("canvas");
    var context = canvas.getContext("2d");
    var imageData = context.createImageData(width, height);
    return imageData;
}



ImageData.prototype.toString = function() {
    var buffer = "";
    var index = 0;
    var mask = 0x00FF;
    push((this.width >> 8) & mask);
    push(this.width & mask);
    push((this.height >> 8) & mask);
    push(this.height & mask);

    var data = this.data;
    var value;
    for (var i = 0; i < data.length; i += 4) {
        value = data[i];
        value += data[i + 1] << 8;
        push(value);
        value = data[i + 2];
        value += data[i + 3] << 8;
        push(value);
    }

    return buffer;

    function push(value) {
        buffer += String.fromCharCode(value);
    }
}

ImageData.fromString = function(stringValue) {
    var index = 0;
    var width = next() << 8;
    width += next();
    var height = next() << 8;
    height += next();
    var imageData = ImageData.create(width, height);

    var data = imageData.data;
    var value, i;
    var mask = 0xFF;
    for (i = 0; i < data.length; i+=4) {
        value = next();
        data[i] = value & mask;
        value >>= 8;
        data[i + 1] = value & mask;
        value = next();
        data[i + 2] = value & mask;
        value >>= 8;
        data[i + 3] = value & mask;
    }
    return imageData;
    function next() {
        return stringValue.charCodeAt(index++);
    }
}

ImageData.loadFromFile = function(src, scale, callBack) {
    if (scale == undefined)
        scale = 1;
    var localName = "image:" + src + "#" + scale;
    var imageData;
    if (localStorage[localName] != undefined) {
        imageData = ImageData.fromString(localStorage.getItem(localName));
        if (callBack != undefined) {
            callBack(imageData);
        }
        return true;
    }
    var image = new Image();
    image.onload = function () {
        var canvas = document.createElement("canvas");
        canvas.width = this.width*scale;
        canvas.height = this.height*scale;
        var context = canvas.getContext("2d");
        context.drawImage(this, 0, 0, canvas.width, canvas.height);
        var imageData = context.getImageData(0, 0, canvas.width, canvas.height);
        window.localStorage.setItem(localName, imageData.toString());
        if (callBack) {
            callBack(imageData);
        }
    }
    image.src = src;
    return false;
}



