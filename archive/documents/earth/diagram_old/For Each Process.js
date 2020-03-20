function Sequence(inputs) {
	return {
		first : inputs.first,
		second : inputs.second,
		process : Process(
			function(inputs) {
				return {
					first : this.first.play(this.first, inputs),
					second : this.second.play(this.second, inputs)
				}
			}
		)
	}
}
