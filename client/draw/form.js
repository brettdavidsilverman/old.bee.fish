class Form extends Item {
   _div;
   item;

   constructor(input)
   {
      super(input.item);

      this.item = input.item;
      this.parent = this.item;
      
      this.item.form = this;

      this.dimensions = input.item.dimensions;
      
      this.html  = input.html;

   }
   
   toJSON()
   {
      return {
         item: super.toJSON(),
         html: this.html
      }
   }
   
   
   remove() {
      if (this.div) {
         this.removeDiv();
      }
      this.item.form = null;
      super.remove();
   }

   removeDiv() {
      document.body.removeChild(this.div);
      this.div = null;
   }

   createDiv()
   {
      console.log("Create Div");

      var form = this;

      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = "2";
      div.style.backgroundColor = "rgba(256, 256, 0, 0.5)";     
      div.style.border = "1px solid black";

      if (this.html == undefined)
         div.innerHTML = "";
      else
         div.innerHTML = this.html;
      
      div.onblur = function(event) {
         form.html = div.innerHTML;
         form.save();
      }

      this._div = div;

      document.body.appendChild(div);
      
      return div;

   }
   
 
   async draw(context) {

      console.log("Draw");

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