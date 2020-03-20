function pgSort() {

    this.depth = 0;

    this.addRecord = function(bitString) {

        for (var x = this.depth; x < bitString.length; x++) {
            this[0] = [this[0], this[1]];
            delete this[1];
            this.depth++;
        }

        var path = this;

        for(var x = bitString.length; x < this.depth; x++) {
            path = path[0];
        }

        for (var x = 0; x < bitString.length; x++) {
            bit = parseInt(bitString[x]);
            if (path[bit] == undefined)
                path[bit] = new Array();
            path = path[bit];
        }
    }

    this.toString = function () {
        return toString(this);

        function toString(path) {
            if (path == undefined)
                return "0";

            return "1" + 
                toString(path[0]) +
                toString(path[1]);
        }
    }

    return this;
}

function Node(parent) {

    this.parent = parent;
    this.value = undefined;

    this.toString = function() {

        if (this.value == undefined)
            return "0";

        return "1" + this.low.toString() + this.high.toString();

    }

    this.first = function() {

        if (this.low != undefined)
            return this.low.first();

        if (this.high != undefined)
            return this.high.first();

        return this;
    }

    this.next = function() {
        if (this.high != undefined)
            return this.high.first();
        return this.parent.next();
    }

    this.previous = function() {
        if (this.low != undefined)
            return this.low.last();
        return this.parent.previous();
    }
}

pgSort.prototype = new Node();
