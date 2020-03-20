function Element(contexts) {
	Process.call(this, contexts);
	
	if (contexts.document == undefined) {
		contexts.document = new Document_20Context(contexts);
	}

	this.inputs.tag = undefined;
	this.inputs.parent = contexts.document.document.body;

	this.outputs.element = undefined;
	
	documentFunction = new Function(this);
	
	documentFunction.action = function() 
	{
		this.object.outputs.element = contexts.document.document.createElement(this.object.inputs.tag);
		this.object.inputs.parent.appendChild(this.object.outputs.element);
	}

	contexts.document.functions.push(documentFunction);

	return this;
}
