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
   
   drawLabel(context) {

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

   async getColor() {
      if (this.selected) {
         return "yellow";
      }
      else {
         var from = await this.from.fetch();
         if (from.value === undefined)
            return "red";
         else
            return "green";
      }
   }

   async draw(context) {
   
      super.draw(context);

      var color = await this.getColor();

      context.fillStyle = context.strokeStyle = color;

      var height = 20 / context.matrix.scale();
      var width = 10 / context.matrix.scale();

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

      arrow(context, this.fromPoint, this.toPoint, width, height);

      function arrow(context, fromPoint, toPoint, width, height) {
         context.save();
         var angle = Math.atan((toPoint.x - fromPoint.x ) / (toPoint.y - fromPoint.y));
         var matrix = new Matrix(context.matrix);
         matrix.translateSelf(toPoint.x, toPoint.y);
         context.setTransform(matrix);
         context.rotate(-angle);
         
         context.beginPath();
            context.moveTo(0, 0);
            context.lineTo(- width / 2, - height);
            context.lineTo(width / 2, - height);
            context.lineTo(0, 0);
         context.closePath();

         context.fill();
         context.restore();
      }
   }

   async remove() {
      
      var from = await this.from.fetch();
      var to = await this.to.fetch();
      from.outputs.remove(to);
      from.outputConnectors.remove(this);
      from.parent.children.remove(this);
      from.save();
      from.parent.save();
      to.inputs.remove(from);
      to.inputConnectors.remove(this);
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