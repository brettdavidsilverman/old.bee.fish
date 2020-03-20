function ElementEvent() {

	this.element = undefined;
	this.event = undefined;
	this.occur = new Function();
	
	this.action = function() {
		ElementEvent.prototype.coordinator = this.element;
		//ElementEvent.prototype.even = this.
		//this.coordinator[this.event] = this.occur.action();
	}
	
	return this;
}
this.prototype = new Occurence();
