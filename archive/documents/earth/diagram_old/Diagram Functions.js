function Diagram_20Functions(inputs) {
	var outputs = Outputs({inputs : inputs, process : Diagram_20Functions});
	outputs.diagram = function() {
		this.width = 0;
		var context = inputs.diagram.context;
		var line = {codeLine : undefined, x : inputs.x, y : inputs.y, maxWidth : 0};

		for (outputIndex in inputs.process) {
			var output = inputs.process[outputIndex];
			if (typeof(output) == "function") {
				line.codeLine = outputIndex;
				context.font = "14px Monospace bold";
				context.fillStyle="#FF0000";
				line = drawCodeLine(line);
				if (line.width > this.width) {
					this.width = line.width;
				}
				line.y += 14;
				context.font = "14px Monospace";
				context.fillStyle="#000000";
				var codeLines = output.toString().split("\n");
				for (codeLineIndex in codeLines) {
					if (codeLineIndex > 0 && codeLineIndex < codeLines.length - 1) {
						line.codeLine = codeLines[codeLineIndex];
						line = drawCodeLine(line);
						if (line.width > this.width) {
							this.width = line.width;
						}
						line.y += 14;
					}
				}
				line.y += 10;
			}
		}
		this.height = line.y;
		function drawCodeLine(inputs) {
			var outputs = Outputs(inputs);
			outputs.y = inputs.y;
			outputs.x = inputs.x;
			context.fillText(inputs.codeLine, inputs.x, inputs.y);
			outputs.width = context.measureText(inputs.codeLine).width;
			return outputs;
		}
	}

	return outputs;
}
