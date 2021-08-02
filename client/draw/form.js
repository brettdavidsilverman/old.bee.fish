class Form extends Item
{
   div;
   canvas;
   item;

   constructor(input)
   {
      super(input);

      this.canvas = input.canvas;
      this.canvas.children.push(this);
      this.dimensions = input.item.dimensions;
      this.createDiv(this.canvas.matrix);
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
         var element = this.canvas.element;
         element.parentNode.removeChild(this.div);
         this.div = null;
         var children = this.canvas.children;
         children[children.indexOf(this)] = undefined;
      }
   }
   
   createDiv(matrix)
   {
      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = 2;
      div.style.backgroundColor = "blue";//"rgba(128, 128, 0, 0.5)";     
      div.style.border = "1px solid black";

      var dim = this.dimensions.matrixTransform(matrix);

      div.style.left = dim.min.x + "px";
      div.style.top = dim.min.y + "px";
      div.style.width = dim.width + "px";
      div.style.height = dim.height + "px";


      
      this.div = div;
      
      var element = this.canvas.element;
      element.parentNode.insertBefore(div, element.nextSibling);
      
      return div;

   }
   
   
}