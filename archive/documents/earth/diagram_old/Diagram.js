function Diagram(inputs) {

	var outputs = Outputs({inputs : inputs, process : Diagram});

    outputs.create = function()
	{
		var canvas = Canvas();
		canvas.create();
		//this.canvas = canvas.canvas;
		this.context = canvas.canvas.getContext("2d");
        var diagramFunctions = Diagram_20Functions({process : inputs.process, diagram : this, x: 10, y : 20});
		diagramFunctions.diagram();
        canvas = Canvas({height : diagramFunctions.height + 20, width : diagramFunctions.width + 10})
        canvas.create();
        this.context = canvas.canvas.getContext("2d");

        this.context.strokeStyle="#000000";
		this.context.fillStyle = "#000000";
		this.context.font = "8px Arial";
		x = 0;
		y = 8;
		for (property in inputs.process.inputs) {
			input = inputs.process.inputs[property];
			this.context.fillText(property + " ", x, y);
			x += this.context.measureText(property + " ").width;
		}
		x = 0;
		y = canvas.canvas.height - 8;
		for (property in inputs.process) {
			output = inputs.process[property];
			this.context.fillText(property + " ", x, y);
			x += this.context.measureText(property + " ").width;
		}
		diagramFunctions.diagram();

//		this.context.strokeRect(0,0,diagramFunctions.width,diagramFunctions.height);
/*
        actions = this.outputs.action.toString().split("\n");
		for (action in actions) {
			this.context.fillText(actions[action], x, y);
			y += 8;
		}
*/
        canvas.append();
		return this;
	}

	return outputs;
}
