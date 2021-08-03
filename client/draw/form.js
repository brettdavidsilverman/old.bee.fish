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
      this.dimensions = input.item.dimensions;
      //this.createDiv();
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
   
   
   remove() {
      if (this.div) {
         this.removeDiv();
         var children = this.canvas.children;
         children[children.indexOf(this)] = undefined;
      }
   }

   removeDiv() {
      var element = this.canvas.element;
      element.parentNode.removeChild(this.div);
      this.div = null;
   }

   createDiv()
   {

      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = 1;
      div.style.backgroundColor = "rgba(128, 128, 0, 0.5)";     
      div.style.border = "1px solid black";

      var matrix = this.canvas.matrix;
      var dim = this.dimensions.matrixTransform(matrix);

      div.style.left = dim.min.x + "px";
      div.style.top = dim.min.y + "px";
      div.style.width = dim.width + "px";
      div.style.height = dim.height + "px";


      
      this._div = div;
      
      var element = this.canvas.element;
      element.parentNode.insertBefore(div, element.nextSibling);
      
      return div;

   }
   
 
   async draw(context) {

      var draw = await super.draw(context);

      if (draw) {
         if (this.div == undefined) {
            this.createDiv();
         }
      }
      else {
         if (this.div) {
            this.removeDiv();
         }
      }

      if (draw) {
         var matrix = context.matrix;
         var dim = this.dimensions.matrixTransform(matrix);
         var div = this.div;
         div.style.left = dim.min.x + "px";
         div.style.top = dim.min.y + "px";
         div.style.width = dim.width + "px";
         div.style.height = dim.height + "px";
      }

      return draw;
   }
   
   get div() {
      if (!this._div)
         this.createDiv();
      return this._div;
   }
}