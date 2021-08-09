class Connector extends Line {

   from;
   to;
   fromPoint;
   toPoint;

   constructor(input) {
      super(input ? input.line : null);
      
      this.from = input.from;
      this.to = input.to;

      if (input.fromPoint)
         this.fromPoint  = new Point(input.fromPoint);
      else
         this.fromPoint = this.from.dimensions.center;

      if (input.toPoint)
         this.toPoint = new Point(input.toPoint);
      else
         this.toPoint = this.to.dimensions.center;
   }
   

   toJSON() {
      return {
         line: super.toJSON(),
         from: this.from.key,
         to: this.to.key,
         fromPoint: this.fromPoint,
         toPoint: this.toPoint
      }
   }

   draw(context) {
   
      super.draw(context);
      
      if (this.selected) {
         context.fillStyle = 
            context.strokeStyle = "yellow";
      }
      else {
         context.fillStyle = 
            context.strokeStyle = "green";
      }
            
      var radius = 10 / context.matrix.scale();

      context.lineWidth = 2 / context.matrix.scale();

      context.beginPath();
      

      context.moveTo(
         this.fromPoint.x,
         this.fromPoint.y
      );

      context.lineTo(
         this.toPoint.x,
         this.toPoint.y
      );

      context.stroke();

      arrow(context, this.fromPoint.x, this.fromPoint.y, this.toPoint.x, this.toPoint.y, radius);

      function arrow(context, fromx, fromy, tox, toy, r){
         var x_center = tox;
         var y_center = toy;
         
         var angle;
         var x;
         var y;
         
         context.beginPath();
         
         angle = Math.atan2(toy-fromy,tox-fromx)
         x = r*Math.cos(angle) + x_center;
         y = r*Math.sin(angle) + y_center;
      
         context.moveTo(x, y);
         
         angle += (1/3)*(2*Math.PI)
         x = r*Math.cos(angle) + x_center;
         y = r*Math.sin(angle) + y_center;
         
         context.lineTo(x, y);
         
         angle += (1/3)*(2*Math.PI)
         x = r*Math.cos(angle) + x_center;
         y = r*Math.sin(angle) + y_center;
         
         context.lineTo(x, y);
         
         context.closePath();
         
         context.fill();
      }
   }
/*   
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
 */  
   
}