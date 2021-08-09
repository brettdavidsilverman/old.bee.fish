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
      
      var to = this.points[
         this.points.length - 1
      ];
      
      if (this.selected) {
         context.strokeStyle = "yellow";
      }
      else {
         context.strokeStyle = "green";
      }
            
      var radius = 3;
   
      context.beginPath();
      context.moveTo(
         this.fromPoint.x,
         this.fromPoint.y
      );

      context.lineTo(
         this.toPoint.x,
         this.toPoint.y
      );

      context.arc(
         this.toPoint.x,
         this.toPoint.y,
         radius,
         0,
         2 * Math.PI
      );

      context.stroke();
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