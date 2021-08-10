class Connector extends Line {

   from;
   to;
   fromPoint;
   toPoint;

   constructor(input) {
      super(input ? input.line : null);
      
      var from, to;

      if (input.from instanceof Item)
         from = input.from;

      if (input.to instanceof Item)
         to = input.to;

      this.from = new ChildPointer({parent: this, pointer: input.from});
      this.to = new ChildPointer({parent: this, pointer: input.to});

      if (input.fromPoint)
         this.fromPoint  = new Point(input.fromPoint);
      else if (from)
         this.fromPoint = from.dimensions.center;

      if (input.toPoint)
         this.toPoint = new Point(input.toPoint);
      else if (to)
         this.toPoint = to.dimensions.center;

      var min = Point.min(this.fromPoint, this.toPoint);
      var max = Point.max(this.fromPoint, this.toPoint);

      this.dimensions = new Dimensions(
         {
            min,
            max
         }
      );

   }
   

   toJSON() {
      return {
         line: super.toJSON(),
         from: this.from,
         to: this.to,
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

   async remove() {
      
      var from = await this.from.fetch();
      var to = await this.to.fetch();
      from.outputs.remove(to.key);
      from.save();
      to.inputs.remove(from.key);
      to.save();
      this.parent.children.remove(this);
      this.parent.save();
      super.remove();
   }
   
/*   
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