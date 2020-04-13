function App(input) {
   var app = this;
   
   this.id;
   
   Drawing.call(this, input);
   
   if (this.statement == null)
      this.statement = "this.promptStatement()";
   
   if (this.inConnectors == null)
      this.inConnectors = [];
   
   if (this.outConnectors == null)
      this.outConnectors = [];
      
   if (this.input == null)
      this.input = {}
      
   this.click = function(point) {

      if (this.f) {
         this.runFunction();
      }

      return true;
   }
   
   this.longPress = function(point) {
   
      // get the editor
      this.editor =
         this.canvas.layers.push();
      
      this.editor.point = point;
      
      selection = this;
      
      this.canvas.draw();
      
   }


   var Drawing_draw = this.draw;
   
   this.draw = function(context) {
         
      if (Drawing_draw.call(
              this,
              context) == false)
         return false;
      
      if (this.parent != null)
         this.drawLabel(context);
      
      return true;
   }
   
   this.drawLabel = function(context) {
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
      
   this.drawFrame = function(context) {
   
      var draw =
         ( (this.parent != null &&
            this.parent == this.layer )
            || this.selected );

      if (draw == false)
         return false;
      
      this.setStyle(this.frame);
      
      this.frame.draw(context);
      
      return true;
      
   }
  
   // set the fill style 
   // and stroke style for this 
   // conte t
   this.setStyle = function(context) {
   
      const alpha = 0.5;
      
      if ( this.selected ) {
         if (this.f != null &&
             this.output != null) {
            // purple
            context.fillStyle = rgba(127, 0, 127, alpha);
            context.strokeStyle = "purple";
         }
         else {
            // yellow
           context.fillStyle = rgba(255, 255, 63, alpha);
           context.strokeStyle = "yellow";
         }
      }
      else if (this.error != null) {
         // red
         context.fillStyle = rgba(255, 0, 0, alpha);
         context.strokeStyle = "red";
      }
      else if (this.f != null &&
                  this.output == null) {
         // green
         context.fillStyle = rgba(127, 255, 127, alpha);
         context.strokeStyle = "green";
      }
      else if (this.f != null &&
                  this.output != null) {
         // blue
         context.fillStyle = rgba(127, 127, 255, alpha);
         context.strokeStyle = "blue";
      }
      else if (this.f == null) {
         // grey
         context.fillStyle = rgba(127, 127, 127, alpha);
         context.strokeStyle = "grey";
      }
      else
         console.log("Invalid state");

      function rgba(red, green, blue, alpha) {
         return "rgba(" +
            red.toString() + "," +
            green.toString() + "," +
            blue.toString() + "," +
            alpha.toString() + ")";
      }
   }
   
   this.promptStatement = function() {
   
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
   
   this.promptLabel = function() {
   
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
   
   this.promptText = function() {
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

   this.promptConsole = function() {
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
   
   this.createFunction = function(keepOutput) {
   
      
      delete this.f;
      delete this.error;
      if (!keepOutput)
         delete this.output;
      
      if (this.statement == null)
         return false;
      
      if (this.statement == "")
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
   
   this.runFunction = function() {
   
      delete this.output;
      delete this.error;

      try {
         var input = this.input;
         output = this.f(input);
         connectOutputs(this, output);
      }
      catch (error) {
         this.error = error;
         alert(this.error);
         return;
      }
      
      this.save();
      this.canvas.draw();
      
      return true;
   }
   
   this.addOutConnector = function(connector) {
      this.outConnectors.push(connector);
   }
   
   this.addInConnector = function(connector) {
      this.inConnectors.push(connector);
   }
   
   this.removeOutConnector = function(connector) {
      removeItem(this.outConnectors, connector);
   }
   
   this.removeInConnector = function(connector) {
      removeItem(this.inConnectors, connector);
   }
   
   function removeItem(array, item) {
      var index = array.indexOf(item);
      array.splice(index, 1);
   }
   
   function connectOutputs(app, output) {
      app.outConnectors.forEach(
         function(connector) {
            connector.connectOutput(output);
         }
      );
      app.output = output;
   }
   
   this.createFunction(true);

}