function Function_20Statement(inputs, statements) {
	statements.javascript(1);
	var statement = "function(" + inputs.join(",") + ") {\n" 
		+ "\t" + contexts.javascript.join("\n\t") + "\n"
		+ "}";
	delete this.context;
	
	this.inputs = inputs;
	this.statements = statements;
	this.javascript = function(tabs) {
		this.contexts.javascript.push(Array(tabs + 1).join("\t") + "function (" + this.inputs.join(",") + ")");
		this.statements.javascript(tabs + 1);
	}
}