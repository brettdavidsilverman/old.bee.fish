class Form extends Item {
   _div;
   _editing;
   _attached = false;
   html;

   constructor(input)
   {
      super(input ? input.item : null);

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

   }

   createDiv()
   {
      if (this._div)
         return;

      var form = this;

      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = "2";
      div.style.backgroundColor = "rgba(256, 256, 256, 0.5)";     
      div.style.border = "1px solid black";

      if (this.html == undefined)
         div.innerHTML = "";
      else
         div.innerHTML = this.html;
      
      div.onblur = function(event) {
         form.editing = false;
      }

      this._div = div;

      this._attached = false;

      // Set editing properties
      this.editing = this._editing;

      
      return div;

   }
   
   removeDiv() {
      if (this._div) {
         this.blur();
         document.body.removeChild(this._div);
         this._div = null;
         this._attached = false;
      }
   }

   async hide() {
      super.hide();
      this.editing = false;
      this.removeDiv();
   }

   show() {
      super.show();
      this.createDiv();
   }
 
   blur() {
      this.editing = false;
   }

   focus() {
      this.show();
      this.div.focus();
   }

   async draw(context) {

      var draw = await super.draw(context);

      if (draw) {

         var div = this._div;

         var matrix = context.matrix;
         var dim = this.dimensions.matrixTransform(matrix);
         div.style.left = dim.min.x + "px";
         div.style.top = dim.min.y + "px";
         div.style.width = dim.width + "px";
         div.style.height = dim.height + "px";

         if (!this._attached) {
            document.body.appendChild(div);
            this._attached = true;
         }

      }

      return draw;
   }
   
   click(point) {
      alert(this.html);
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
            div.innerText = div.innerHTML;
            div.contentEditable = true;
            div.classList.add("editing");
            div.focus();
         }
         else {
            div.contentEditable = false;
            div.style.zIndex = "0";
            div.classList.remove("editing");
            if (this._editing) {
               this.html = div.innerText;
               this.save();
               div.innerHTML = this.html;
            }
         }
      }
      this._editing = value;
   } 
}