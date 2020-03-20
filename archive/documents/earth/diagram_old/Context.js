function Context() {
	this.functions = new Array();
	this.action = function() {
		for (func in this.functions) {
			this.functions[func].action();
		}
	}
}