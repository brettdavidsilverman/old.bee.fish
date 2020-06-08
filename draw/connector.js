class Connector extends Line {
   
   constructor(input) {
      super(input);
      
      this.from = input.from;
      this.to = input.to;

      if (!input.statement)
         this.statement = "return input != null;";
   
      if (!input.connect)
         input.connect = true;
      
      if (!input.label)
         this.label = null;
         
      this.createFunction(true);
 

   }
   
   
   setStyle(context) {
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
   
   drawFrame(context) {
      if (this.selected)
         this.frame.draw(context);
   }
   
   draw(context) {
   
      super.draw(context);
      
      var to = this.getPoint(
         this.points.length - 4
      );
      
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
   
   promptConditional() {
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
   
   remove() {
       this.from.removeOutConnector(this);
       this.to.removeInConnector(this);
       super.remove();
   }
   
   click(point) {
      this.connectOutput(this.from.output);
   }
   

   connectOutput(output) {
      var label = this.label;
      if (label == null)
         label = this.from.label;

      var connect = true;
      if (this.f) {
         connect = this.f(output);
      }
            
      this.connect = connect;
      
      if (connect) {
         this.to.input[label] = output;
         this.to.output = null;
      }
      
      return connect;
      
   }
   
   
}