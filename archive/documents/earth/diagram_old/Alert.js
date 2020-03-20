function Alert(inputs) {
	
	var outputs = Outputs(inputs);
	
	outputs.show = function () {
		window.alert(inputs.message);
	}
		
	return outputs;
}
