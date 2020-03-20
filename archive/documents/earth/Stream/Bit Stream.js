BitStream = function() {

    var array = this;
    this.read = function(stream) {

        for (var position = 0;
             position < this.length;
             position++) {
            if (stream.write(this[position], this[position +1]))
                position++;
        }

    }

    this.toNumber = function() {
        var number = 0;
        var read = new this.Read();
        for (bit = read.next(); bit != undefined; bit = read.next())
            number = (number << 1) | bit;
        return number;
    }

}
BitStream.prototype = Array.prototype;
BitStream.Write = function() {
    this.value = new BitStream();
    this.write = function(bit, next) {
        this.value.push(bit);
    }
}
