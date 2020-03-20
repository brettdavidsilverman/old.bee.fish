function Script() {

	this.inputs.type = "text/javascript";
	this.inputs.src = undefined;
	
	var script = this;
	
	this.Document = function() {
		this.action = function() {
			script.Document.prototype.action();
			var context = this.outputs.div.canvas.context;
			context.fillStyle="#FFFFFF";
			context.beginPath();
			context.arc(50,50,15,0,Math.PI*2,true);
			context.fill();
		}
		return this;
	}
	this.Document.prototype = new Diagram();
	
	this.action = function() {
		var script = this.inputs.document.createElement("script");
		script.src = this.inputs.src;
		script.type = this.inputs.type == undefined ? "text/javascript" : this.inputs.type;
		this.inputs.document.body.appendChild(script);
		this.outputs.script = script;
	}
	return this;
}
Script.prototype = new Document();
