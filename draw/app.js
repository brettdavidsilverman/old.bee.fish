class App extends Drawing {
   _label;
   
   constructor(input) {
      super(input);
      
      this["="];
   
      if (!input.statement)
         this.statement = "this.promptStatement()";
   
      if (!input.inConnectors)
         this.inConnectors = [];
   
      if (!input.outConnectors)
         this.outConnectors = [];
      
      if (!input.input)
         this.input = {}
         
      if (input.statement)
         this.createFunction(true);
         
      if (input._label)
         this.label = input._label;
   }
   
   click(point) {

      if (this.f) {
         this.runFunction();
      }

      return true;
   }
   
   longPress(point) {
      
      this.canvas.selection = this;

      // get the editor
      this.editor =
         this.canvas.layers.push();
      
      this.editor.point = point;
      
      
      
      this.canvas.draw();
      
   }

   draw(context) {
      if (!super.draw(context))
         return false;
      
      if (this.parent != null)
         this.drawLabel(context);
      
      return true;
   }
  
   get label() {
      if (this._label)
         return this._label;
      return this["="].name;
   }
   
   set label(value) {
      this._label = value;
   }
   
   drawLabel(context) {
      var label = this.label;
      if (label == null)
         return;
      
      context.save();
      
      context.fillStyle = "black";
      context.strokeStyle = "black";

      var dimensions =
         this.dimensions;
            
      var topLeft =
         dimensions.topLeft;
         
      var scale = context._scale;
      var fontSize = 3; //mm
      
      context.font =
         String(fontSize) + "mm" +
         " Courier new";
      
      context.scale(1, -1);

      var width = context.measureText(
         this.label
      ).width;
      
      var x = topLeft.x        +
        dimensions.width   / 2 -
        width              / 2;
        
      var y = topLeft.y        -
         dimensions.height / 2 -
         fontSize;
         
      context.beginPath();
         
      context.fillText(
         label,
         x,
         -y
      );
      context.stroke();
         
      context.restore();
   }
   
   
   // set the fill style 
   // and stroke style for this 
   // context
   setStyle(app) {
   
      const alpha = 0.5;
      
      if ( this.selected ) {
         if (this.f &&
             this.output != null) {
            
            // yellow
            app.fillStyle = rgba(255, 255, 63, alpha);
            app.strokeStyle = "yellow";
      
         }
         else {
            // purple
            app.fillStyle = rgba(127, 0, 127, alpha);
            app.strokeStyle = "purple";
         }
      }
      else if (this.error != null) {
         // red
         app.fillStyle = rgba(255, 0, 0, alpha);
         app.strokeStyle = "red";
      }
      else if (this.f  &&
               this.output == null) {
         // orange
         app.fillStyle = rgba(127, 63, 0, alpha);
         app.strokeStyle = "orange";
      }
      else if (this.f != null &&
                  this.output != null) {
         // green
         app.fillStyle = rgba(0, 127, 0, alpha);
         app.strokeStyle = "green";
      }
      else if (this.f == null) {
         // black
         app.fillStyle = rgba(127, 127, 127, alpha);
         app.strokeStyle = "black";
      }
      else
         throw new Error("Invalid state");
      
      app.lineWidth = 0.5;
      
      function rgba(red, green, blue, alpha) {
         var string =
            "rgba(" +
            red.toString() + "," +
            green.toString() + "," +
            blue.toString() + "," +
            alpha.toString() + ")";
         return string;
      }
   }
   
   promptStatement() {
   
      var statement =
         prompt(
            "Statement",
             this.statement
         );
      
      if (statement == null)
         // user cancelled
         return false;
         
      this.statement = statement;
      
      this.createFunction();
      
      this.canvas.draw();
      
      return true;
   }
   
   promptLabel() {
   
      var label =
         prompt("Label?", this.label);

      if (label == null)
         return;
         
      if (label == "")
         this.label = null;
      else
         this.label = label;
         
      this.canvas.draw();
   }
   
   promptText() {
      var text = prompt("Text?");
      if (text == null)
         return false;
      var object = new Text(
         {
            text: text,
            point: this.editor.point,
            parent: this
         }
      );
      this.addChild(object);
      return true;
   }

   promptConsole() {
      if (this.lastCommand == null)
         this.lastCommand = "console.log(this)";
         
      var command = prompt("Command?", this.lastCommand);
      if (command == null)
         return false;
      
      this.lastCommand = command;
      
      try {
         eval(command)
      }
      catch(error) {
         alert(error)
         return false;
      }
      
      return true;
   }
   
   createFunction(keepOutput) {
   
      delete this.f;
      delete this.error;
      if (!keepOutput)
         delete this.output;
      
      if (!this.statement)
         return false;
    
      try {
         this.f = new Function(
            "input",
            this.statement
         );
      }
      catch (error) {
         this.error = error;
         alert(this.error)
      }
      
      return true;
   }
   
   runFunction() {
   
      delete this.output;
      delete this.error;

      try {
         var input = this.input;
         var output = this.f(input);
         this.connectOutputs(this, output);
      }
      catch (error) {
         this.error = error;
         alert(this.error);
         return;
      }
      
      this.canvas.draw();
      
      return true;
   }
   
   addOutConnector(connector) {
      this.outConnectors.push(connector);
   }
   
   addInConnector(connector) {
      this.inConnectors.push(connector);
   }
   
   removeOutConnector(connector) {
      removeItem(this.outConnectors, connector);
   }
   
   removeInConnector(connector) {
      removeItem(this.inConnectors, connector);
   }
   
   removeItem(array, item) {
      var index = array.indexOf(item);
      array.splice(index, 1);
   }
   
   connectOutputs(app, output) {
      app.outConnectors.forEach(
         function(connector) {
            connector.connectOutput(output);
         }
      );
      app.output = output;
   }
   

}