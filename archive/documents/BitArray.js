function BitArray() {
    this.isBitArray = true;
    this.toNumber = function() {
        var number = 0;
        for (var bit = 0; bit < this.length; bit++) {
            number = (number << 1) | this[bit];
        }
        return number;
    }
}
BitArray.prototype = Array.prototype;

Number.prototype.toBitArray = function () {
    // convert number to string of base 2
    var string = this.toString(2);

    // convert string to bit array
    var bitArray = new BitArray();
    for (bit in string) {
        var number = parseInt(string[bit]);
        bitArray.push(number);
    }
    return bitArray;
}
