function Statement(statement) {
	this.statement = statement;
	return this;
}

Statement.prototype.contexts = new Object(this.contexts);

Statement.prototype.play = function(thisArg) {
		if (this.contexts.javascript != undefined) {
			this.javascript(0);
		}
		if (this.contexts.ignore == undefined) {
	 		eval.call(thisArg, this.statement);
		}
	}
	
Statement.prototype.javascript = function(tabs) {
	this.contexts.javascript.push(Array(tabs + 1).join("\t") + this.statement + "\n");
}
