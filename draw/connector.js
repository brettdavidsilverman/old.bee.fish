function Connector(input) {
   var connector = this;
   
   this.from = input.from;
   this.to = input.to;


   if (input.statement == null)
      this.statement = "this.promptConditional()";
   
   if (input.connect == null)
      input.connect = true;
      
   Line.call(this, input);
   
   this.setStyle = function(context) {
      if (this.selected) {
         context.strokeStyle = "yellow";
         context.fillStyle = "rgba(0,127,127,0.3)";
      }
      else if (this.connect) {
         context.strokeStyle = "green";
         context.fillStyle = "rgba(0,127,0,0.3)";
      } else {
         context.strokeStyle = "red";
         context.fillStyle = "rgba(127,0,0,0.3)";
      }
      
      var scale = context._scale;
      context.lineWidth = 0.5 / scale;
   }
   
   this.drawFrame = function(context) {
      if (this.selected)
         this.frame.draw(context);
   }
   
   var Line_draw = this.draw;
   this.draw = function(context) {
      Line_draw.call(this, context);
      var to = this.points[
         this.points.length - 1
      ];
      
      this.setStyle(context);
      
      var radius = 3;
 
      context.beginPath();
      context.arc(
         to.x,
         to.y,
         radius,
         0,
         2 * Math.PI
      );
      context.fill();
      context.stroke();
   }
   
   this.promptConditional = function() {
      var tryAgain = true;
      while (tryAgain) {
         var statement = this.statement;
         var statement =
            prompt(
               "Conditional statement?",
                statement
            );
         if (statement == null)
            return false;
         if (statement == "") {
            this.statement = "";
            delete this.f;
            return true;
         }
         
         this.statement = statement;
         
         if (!this.createFunction())
            tryAgain = true;
         else
            tryAgain = false;
      }
      return true;
   }
   
   var Drawing_remove = this.remove;
   
   this.remove = function() {
       // Connect from -> outputs
       this.from.removeOutConnector(this);
       this.to.removeInConnector(this);
       Drawing_remove.call(this);
   }
   
   this.click = function(point) {
      this.connectOutput(this.from.output);
   }
   
   this.connectOutput = function(output) {
      var label = this.label;
      if (label == null)
         label = this.from.label;
      if (label == null)
          return;
               
      var connect = true;
      if (this.f) {
         try {
            connect = this.f(output);
         }
         catch (error) {
            this.connect = false;
            alert(error);
            return false;
         }
      }
            
      this.connect = connect;
      if (connect) {
         this.to.input[label] = output;
         this.to.output = null;
      }
      
      return connect;
      
   }
   
}