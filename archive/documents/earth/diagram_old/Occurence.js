function Occurence() {

	this.coordinator = undefined;
	this.event = undefined;
	this.occur = new Function();
	
	this.action = function() {
		this.coordinator[this.event] = this.occur.action;
	}
	
	return this;
}
this.prototype = new Function();