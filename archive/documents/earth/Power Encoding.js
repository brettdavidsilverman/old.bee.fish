function PowerEncoding(value) {

    if (value != undefined) {
        // Get the type of the value
        this.type = PowerEncoding[typeof(value)];
        // Encode the value
        this.binaryTree = PowerEncoding.encode(new this.type(value));
    }

}

PowerEncoding.encode = function(value) {

    if (value.isZero)
        return undefined;

    return new BinaryTree(undefined, PowerEncoding.encode(value.power), PowerEncoding.encode(value.remainder));

}


PowerEncoding.prototype.decode = function() {

    return decode(this.type, this.binaryTree);

    function decode(type, binaryTree) {

        if (binaryTree == undefined)
            return type.zero;

        var power = decode(type, binaryTree.left);
        var remainder = decode(type, binaryTree.right);

        return type.add(power, remainder);
    }
}

PowerEncoding.base = 2;

PowerEncoding.prototype.write = function(stream) {

    write(this.binaryTree, stream);

    function write(binaryTree, stream) {
        if (binaryTree == undefined)
            stream.write(0);
        else if (binaryTree.left == undefined && binaryTree.right == undefined) {
            stream.write(1);
            stream.write(0);
        }
        else {
            stream.write(1);
            write(binaryTree.left, stream);
            write(binaryTree.right, stream);
        }
    }
}

PowerEncoding.read = function (type, stream) {

    var powerEncoding = new PowerEncoding();
    powerEncoding.type = type;
    powerEncoding.binaryTree = readBinaryTree(stream);

    function readBinaryTree(stream, parent) {
        if (stream.read() == 0)
            return undefined;
        else if (stream.peek == 0) {
            stream.read();
            return new BinaryTree(parent);
        }
        else {
            var binaryTree = new BinaryTree(parent);
            binaryTree.left = readBinaryTree(stream, binaryTree);
            binaryTree.right = readBinaryTree(stream, binaryTree);
            return binaryTree;
        }
    }
    return powerEncoding;
}

PowerEncoding.number = function (number) {

    this.number = number;

    if (number == 0) {
        this.isZero = true;
    }
    else {
        // Convert the number to the correct base
        var numberString = number.toString(PowerEncoding.base);

        // Calculate the power and remainder
        var power = numberString.length - 1;
        var remainder = number - Math.pow(PowerEncoding.base, power);

        // Store the power and remainder
        this.power = new PowerEncoding.number(power);
        this.remainder = new PowerEncoding.number(remainder);
    }

}

PowerEncoding.number.zero = 0;
PowerEncoding.number.add = function(number, remainder) {
    return Math.pow(PowerEncoding.base, number) + remainder;
}
