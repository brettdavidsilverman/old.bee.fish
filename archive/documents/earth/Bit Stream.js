BitStream = function() {

    this.array = [];

}

BitStream.prototype.pop = function() {
    return this.array.pop();
}

BitStream.prototype.toNumber = function() {
    var number = 0;
    var readStream = this.openRead();
    for (bit = readStream.read(); bit != undefined; bit = readStream.read())
        number = (number << 1) | bit;
    return number;
}

BitStream.prototype.write = function(bit) {
    this.array.push(bit);
}

BitStream.prototype.openRead = function() {
    return new BitStream.ReadStream(this);
}

BitStream.ReadStream = function(bitStream) {

    this.bitStream = bitStream;
    this.position = 0;
    this.isEnd = (bitStream.array.length == 0);
    this.peek = bitStream.array[0];

}

BitStream.ReadStream.prototype.read = function() {
    this.isEnd = (this.position == (this.bitStream.array.length - 1));
    this.peek = this.bitStream.array[this.position + 1];
    return this.bitStream.array[this.position++];
}