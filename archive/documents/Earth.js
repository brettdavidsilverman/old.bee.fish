function Earth(parent) {

    this.parent = parent;
    this.low = undefined;
    this.high = undefined;
    this.paths = function(path) {
/*
        if (paths == undefined)
           paths = new Array();

        if (path == undefined) {
           return paths;
        }

        paths.push(path);
*/
        // convert path to bit array
        if (path.isBitArray != true)
            path = path.toBitArray();

        // follow the path
        var earth = this;

        for(var position = 0; position < path.length; position++) {
            var bit = path[position];
            if (bit == 0) {
                if (earth.low == undefined)
                    earth.low = new Earth(earth);
                earth = earth.low;
            }
            else {
                if (earth.high == undefined)
                    earth.high = new Earth(earth);
                earth = earth.high;
            }
        }
        return earth;
    }

    function path(earth, bitArray) {
        if (earth.parent != undefined) {
            path(earth.parent, bitArray);
            if (earth == earth.parent.low)
                bitArray.push(0);
            else
                bitArray.push(1);
        }
    }

    this.path = function() {
        var bitArray = new BitArray();
        path(this, bitArray);
        return bitArray;
    }

    this.bit = function() {
        if (this.parent == undefined)
            return undefined;

        if (this.parent.low == this)
            return 0;
        else
            return 1;
    }

    this.next = function() {

        var next = this;

        while (next.parent != undefined
          && (next.parent.high == undefined
          || next.parent.high == next)) {
            next = next.parent;
        }

        if (next.parent == undefined)
            return undefined;

        if (next.parent.high != undefined)
            return next.parent.high.first();

        return undefined;
    }

    this.previous = function () {
        if (this.low != undefined)
            return this.low.last();
        return this.parent.previous();
    }

    this.first = function() {

        if (this.low != undefined)
            return this.low.first();

        if (this.high != undefined)
            return this.high.first();

        return this;
    }

    this.last = function() {

        if (this.high != undefined)
            return this.high.last();

        if (this.low != undefined)
            return this.low.last();

        return this;
    }

    this.toBitArray = function(bitArray) {

        if (bitArray == undefined)
            bitArray = new BitArray();

        bitArray.push(1);

        if (0 &&this.bit() == 0 &&
            this.high == undefined &&
            this.low == undefined) {
            bitArray.push(0);
            return bitArray;
        }
        
        if (this.low == undefined)
            bitArray.push(0);
        else
            this.low.toBitArray(bitArray);

        if (this.high == undefined)
            bitArray.push(0);
        else 
            this.high.toBitArray(bitArray);

        return bitArray;

    }

    return this;
}


Earth.fromBitArray = function(bitArray, parent, path) {
    if (bitArray.position == undefined) {
        bitArray.position = 0;
//alert("start");
    }

    var bit = bitArray[bitArray.position++];
    var nextBit = bitArray[bitArray.position];
//alert([bitArray.position - 1, path, bit, nextBit]);

    if (bit == 0) {
//alert("undef");
        return undefined;
    }
    var earth = new Earth(parent);

    if (0 && nextBit == 0 && path == 0) {
alert("00");
        bitArray.position++;
  //      earth.high = Earth.fromBitArray(bitArray, earth, 0);
    }
    else {
//alert("low");
        earth.low = Earth.fromBitArray(bitArray, earth, 0);
//alert("high");
        earth.high = Earth.fromBitArray(bitArray, earth, 1);
    }
    return earth;
}


var encoding = new PowerEncoding();
var earth = new Earth();

for (var x = 0; x < Math.pow(2, 8); x++) {
    earth.paths(x.encode());
    //alert(x.encode());
}
var saved = earth.toBitArray();
delete earth;
earth = Earth.fromBitArray(saved);
delete saved;
var y = 0;
for (earth = earth.first();
     earth != undefined;
     earth = earth.next()) {
    var x = encoding.parse(earth.path()).decode();
    document.writeln(x.toString() + "</br>");
    if (x != y++)
        alert([x, earth.path().join("-")]);
    // alert(earth.path());
}
alert([saved.length, y * 8]);
