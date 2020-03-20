function Statements(statements) {
	this.statements = statements;
}

Statements.prototype.play = function(thisArg) {
	for (statement in this.statements) {
		this.statements[statement].play(thisArg);
	}
}

