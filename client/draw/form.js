class Form extends Item
{
   _div;
   canvas;
   item;

   constructor(input)
   {
      super(input);

      this.canvas = input.canvas;
      this.canvas.children.push(this);
      this.dimensions = input.dimensions;
      this.createDiv();
   }
   
   toJSON()
   {
      return {
         index: this.index,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children,
         point: this.point,
         width: this.width,
         height: this.height
      }
   }
   
   
   async draw(context)
   {
      super.draw(context);

      var matrix =this.canvas.matrix.multiply( this.matrix );

      var dim = this.dimensions.matrixTransform(matrix);
      var div = this._div;

      div.style.left = dim.min.x + "px";
      div.style.top = dim.min.y + "px";
      div.style.width = dim.width + "px";
      div.style.height = dim.height + "px";

      div.style.transform = matrix.toString();
     //    matrix.toString();
     // console.log(this._div.style.border);

   }

   remove() {
      if (this._div) {
         document.body.removeChild(this._div);
         this._div = null;
         var children = this.canvas.children;
         children[children.indexOf(this)] = undefined;
      }
   }
   
   createDiv()
   {
      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = 2;
      div.style.backgroundColor = "blue";//"rgba(128, 128, 0, 0.5)";     
      div.style.border = "1px solid black";

      
      this._div = div;
      
      document.body.appendChild(this._div);
      
      return div;

   }
   
   
}