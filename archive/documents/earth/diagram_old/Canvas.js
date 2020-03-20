function Canvas(inputs) {


    if (inputs == undefined)
        inputs = new Object();

    if (inputs.width == undefined)
        inputs.width = 100;

    if (inputs.height == undefined)
        inputs.height = 100;

    if (inputs.document == undefined)
        inputs.document = window.document;

    var outputs = Outputs({inputs : inputs, process : Canvas});

    outputs.create = function() {
        this.canvas = inputs.document.createElement("canvas");
        this.canvas.width = inputs.width;
        this.canvas.height = inputs.height;
    }

    outputs.remove = function() {
        inputs.document.body.removeChild(this.canvas);
        delete this.canvas;
    }

    outputs.append = function() {
        inputs.document.body.appendChild(this.canvas);
    }

	return outputs;
}