function Vector() {
	this.inputs.size = undefined;
	this.inputs.direction = undefined;
	
	// A vectors action is to simplify 
	// its inputs to the most concise value.
	this.action() = function () {
		this.outputs.value = new Vector();
		this.outputs.value.size = this.inputs.size;
		this.outputs.value.direction = this.inputs.direction;
	}
	return this;
}
Vector.prototype = new Function();
