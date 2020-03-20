function B(number) {
document.writeln(number + "</br>");
    if (number == 0)
        return zero;

    var sign, base, powerSign, power, remainder;
    if (number < 0) {
        number = -number;
        sign = false;
    }
    else {
        sign = true;
    }

    if (number < 1) {
        powerSign = false;
        number = 1/number;
    }
    else
        powerSign = true;

    if (number == 1) {  
        base = 0;
        power = 0;
        remainder = 0;
    }
    else if (number == 2) {
        base = 0;
        power = 0;
        remainder = 1;
    }
    else {
        base = Base(number);
        power = Power(number, base);
        if (powerSign == false) {
            power = -power;
            number = 1/number;
        }
        remainder = Remainder(number, base, power);
    }
 
    this.sign = sign;
    this.base = new B(base);
    this.power = new B(power);
    this.remainder = new B(remainder);
}

function Base(number) {
    var base = 2;
    var size = Size(number, base);;
    while(size > (size = Size(number, base + 1)))
        base++;

    return base;
}

function Size(number, base) {
    var power = Power(number, base);
    var remainder = Remainder(number, base, power);
    return Math.abs(base) + Math.abs(power) + Math.abs(remainder);
}

function Power(number, base) {
    var power = 2;
    while(Remainder(number, base, power) > 0)
        power++;

    if (Math.abs(Remainder(number, base, power-1)) <= Math.abs(Remainder(number, base, power)))
        return power-1;
    else
        return power;
}

function Remainder(number, base, power) {
    return number - Math.pow(base, power);
}

var zero = {
    toNumber : function() {
        return 0;
    },
    toString : function() {
        return "0";
    },
    toBits : function() {
        return [0];
    }
}

B.prototype.toNumber = function() {
    return (this.sign ? 1 : -1) * (Math.pow(this.base.toNumber(), this.power.toNumber()) + this.remainder.toNumber());
}

B.prototype.toString = function() {
    return (this.sign ? "" : "-") + "[" + this.base.toString() + "," + this.power.toString() + "," + this.remainder.toString() + "]";
}

B.prototype.toBits = function() {
    return [1, (this.sign ? 1 : 0)].concat(this.base.toBits(), this.power.toBits(), this.remainder.toBits());   
}

B.fromBits = function(bits) {

    if (bits.shift() == 0)
        return zero;
    
    this.sign = (bits.shift() == 1);
    this.base = new B.fromBits(bits);
    this.power = new B.fromBits(bits);
    this.remainder = new B.fromBits(bits);
}

B.fromBits.prototype = B.prototype;

String.prototype.toNumber = function() {
    var number = 0;
    var power = 0;
    for (var i = this.length - 1; i >= 0; i--) {
        number += this.charCodeAt(i) * Math.pow(2, power);
        power += 8;
    }
    return number;
}

String.fromNumber = function(number) {
    var base16 = number.toString(16);
    var string = "";
    for (var i = base16.length - 1; i >= 0; i -= 4) {
        var code = base16.substring(i - 3, i + 1);
        code = parseInt(code, 16);
        string = String.fromCharCode(code) + string;
    }
    return string;
}
