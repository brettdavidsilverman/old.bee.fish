class Form extends Item {
   _div;
   _editing;

   constructor(input)
   {
      super(input && input.item ? input.item : input);

      this.parent.form = this;

      this.html  = input.html;

      if (this.visible)
         this.show();
   }
   
   toJSON()
   {
      return {
         item: super.toJSON(),
         html: this.html
      }
   }
   
   
   remove() {
      super.remove();
      this.removeDiv();
      this.parent.form = null;
   }

   createDiv()
   {
      if (this._div)
         return;

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

      // Set editing properties
      this.editing = this._editing;

      
      return div;

   }
   
   removeDiv() {
      console.log("Remove Div");
      if (this._div) {
         document.body.removeChild(this._div);
         this._div = null;
      }
   }

   async hide() {
      super.hide();
      this.removeDiv();
   }

   show() {
      super.show();
      this.createDiv();
   }
 
   async draw(context) {

      var draw = await super.draw(context);
/*
      if (draw) {
         console.log("Show");
         if (this._div == undefined) {
            this.createDiv();
         }
      }
      else {
         console.log("Hide");
         if (this._div) {
            this.removeDiv();
         }
      }
*/
      if (draw) {

         var matrix = context.matrix;
         var dim = this.parent.dimensions.matrixTransform(matrix);
         var div = this._div;
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

   get editing() {
      return this._editing;
   }

   set editing(value) {
      var div = this._div;
      if (div) {
         if (value) {
            div.style.zIndex = "3";
            div.contentEditable = true;
            div.focus();
         }
         else {
            div.contentEditable = false;
            div.style.zIndex = "0";
         }
      }
      this._editing = value;
   } 
}