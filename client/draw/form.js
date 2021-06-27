class Form extends Item
{
   _form;
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
         point: this.point,
         width: this.width,
         height: this.height
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
         if (this._form) {
            document.body.removeChild(this._form);
            this._form = null;
         }
         return false;
      }
      
      if (this._form == undefined) {
         this.createForm();
      }
      
      var dim = this.dimensions.matrixTransform(matrix);
      var form = this._form;
      form.style.left = dim.min.x + "px";
      form.style.top = dim.min.y + "px";
      form.style.width = dim.width + "px";
      form.style.height = dim.height + "px";
      //this._form.style.transform = "scale(" + matrix.a + ")";
     //    matrix.toString();
     // console.log(this._form.style.border);
      return super.draw(context);

   }
   
   createForm()
   {
      var form = document.createElement("form");
      form.style.position = "absolute";
      /*
      form.style.top = this.point.y + "px";
      form.style.left = this.point.x + "px";
      form.style.width = this.width + "px";
      form.style.height = this.height + "px";
      */
      form.style.border = "1px solid black";
      
      this._form = form;
      
      document.body.appendChild(this._form);
      
      return form;

   }
   
   
}