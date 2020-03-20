function Context_20Menu() {
	
	this.occurence = new Occurence();
	this.context = new Function();
	
	this.action = function() {
		Context_20Menu.prototype.action.call(this);
		this.occurence.occur.action = function () {
			alert("Hello World");
		}
		this.occurence.action();
	}
	
	return this;
}
Context_20Menu.prototype = new Diagram();