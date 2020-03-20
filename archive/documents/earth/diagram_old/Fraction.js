function Fraction() {
	
	this.inputs.numerator = undefined;
	this.inputs.denominator = undefined;
	
	Number.prototype.divide = function (denominator) {
		return this / denominator;
	}
	
	this.action() = function () {
		this.outputs.value = this.inputs.numerator.divide(this.inputs.denominator);
	}
	
	return this;
	
}

Fraction.prototype = new Function();


