class Form extends Item
{
   _div;
   point;
   width;
   height;
   
   constructor(input)
   {
      super(input);

      if (input.point == undefined)
         this.point = new Point({x:0,y:0});
      else
         this.point = new Point(input.point);
         
      if (input.width == undefined)
         this.width = 200;
      else
         this.width = input.width;
         
      if (input.height == undefined)
         this.height = 400;
      else
         this.height = input.height;
               
      if (!this.dimensioned)
         this.dimensions = 
            this.getDimensions();
      
   }
   
   toJSON()
   {
      return {
         count: this.count,
         selected: this.selected,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children,
         point: this.point
      }
   }
   
   getDimensions() {
      return new Dimensions(
         {
            min: this.point,
            max: new Point(
               {
                  x: this.point.x + this.width,
                  y: this.point.y + this.height
               }
            )
         }
      );
   }
   
   async draw(context)
   {
      var matrix = this.getClippedMatrix(context);
      
      if (!matrix) {
         if (this._div) {
            document.body.removeChild(this._div);
            this._div = null;
         }
         return false;
      }
      
      if (this._div == undefined) {
         this.createDiv();
      }
      
      this._div.style.transform =
         matrix.toString();
      console.log(this._div.style.transform);
      await this.children.draw(context);
      
      return true;
   }
   
   createDiv()
   {
      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.top = this.point.y + "px";
      div.style.left = this.point.x + "px";
      div.style.width = this.width + "px";
      div.style.height = this.height + "px";
      div.style.border = "1px solid black";
      
      this._div = div;
      
      document.body.appendChild(this._div);
      
      return div;

   }
   
   
}