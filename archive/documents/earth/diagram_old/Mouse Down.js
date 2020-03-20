function MouseDown() {


	this.action = function() {
		this.event = "onmousedown";
		MouseDown.prototype.action.call(this);
	}
	
	return this;
}
MouseDown.prototype = new Occurence();