class Drawing extends Rectangle {

   constructor(
      input = {
         lines: []
      }
   ) {
      super(input);

      if (!this.lines)
         this.lines = [];
            
   }
   
   addLine(line) {
      line.setDimensions();
      this.min =
         Point.min(this.min, line.min);
      this.max =
         Point.max(this.max, line.max);
      this.lines.push(line);

      this.resize();
   }
  
   removeLastLine() {
      if (this.lines.length) {
         this.lines.pop();
         this.setDimensions();
         this.resize();
      }
   }
   
   draw(context) {
      super.draw(context);
 
      this.lines.forEach(
         line => line.draw(context)
      );
      
   }
   
   setDimensions() {
      
      var min = new Point(
         {
            x: Number.MAX_VALUE,
            y: Number.MAX_VALUE
         }
      );
      
      var max = new Point(
         {
            x: -Number.MAX_VALUE,
            y: -Number.MAX_VALUE
         }
      );
      
      this.lines.forEach(
         line => {
            min = Point.min(
               min, line.min
            );
            max = Point.max(
               max, line.max
            );
         }
      );

      this.min = min;
      this.max = max;

   }
   
   resize() {

      
      this.topLeft = new Point(
         {
            x: this.min.x,
            y: this.max.y
         }
      );
      
      this.width =
         this.max.x - this.min.x;
      
      this.height =
         this.max.y - this.min.y;
     
      super.resize();
  
   }
}