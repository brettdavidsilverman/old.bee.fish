function PowerEncoding(value) {

    if (value != undefined) {
        // Get the type of the value
        this.type = PowerEncoding[typeof(value)];
        // Encode the value
        this.binaryTree = encode(new this.type(value));
    }

    function encode(value) {

        if (value.isZero) {
            return undefined;
        }

        return new BinaryTree(encode(value.power), encode(value.remainder));
    }

    this.decode = function() {

        return decode(this.type, this.binaryTree);

        function decode(type, binaryTree) {

            if (binaryTree == undefined)
                return type.zero;

            var power = decode(type, binaryTree.left);
            var remainder = decode(type, binaryTree.right);

            return type.add(power, remainder);
        }
    }

    this.write = function(stream) {
        if (this.binaryTree == undefined) {
            stream.read(0);
            return;
        }
        this.binaryTree.write(stream);
    }

}

PowerEncoding.base = 2;

PowerEncoding.Write = function (type) {
    this.values = new Array();
    var values = this.values;
    var type = type;
    function write(binaryTree) {
        var powerEncoding = new PowerEncoding();
        powerEncoding.type = type;
        powerEncoding.binaryTree = binaryTree;
        values.push(powerEncoding);
    }
    this.chain(write);

}
PowerEncoding.Write.prototype = new BinaryTree.Write();

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
