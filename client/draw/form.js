class Form extends Line {
   div;
   _editing;
   html;
   borderColor = "blue";

   constructor(input)
   {
      super(input ? input.line : null);

      if (input == undefined)
         input = {}
        
      this.html  = input.html;

      if (input.borderColor)
         this.borderColor = input.borderColor;
   }
   
   toJSON()
   {
      return {
         html: this.html,
         borderColor: this.borderColor,
         line: super.toJSON()
      }
   }
   
   
   remove() {
      super.remove();
      this.removeDiv();

   }

   createDiv()
   {
      if (this.div)
         return;

      var form = this;

      var div = document.createElement("div");
      div.style.position = "absolute";
      div.style.zIndex = "2";
      div.style.backgroundColor = "rgba(0, 0, 0, 0.5)";     
      div.style.color = "white";
      div.style.border = "1px solid " + this.borderColor;

      if (this.html == undefined)
         div.innerHTML = "";
      else {
         div.appendChild(
            document.createTextNode(this.html)
         );
      }
      
      div.onblur = function(event) {
         form.editing = false;
      }

      this.div = div;

      // Set editing properties
      this.editing = this._editing;
      document.body.appendChild(div);
      
      return div;

   }
   
   removeDiv() {
      if (this.div) {
         this.blur();
         document.body.removeChild(this.div);
         this.div = null;
      }
   }

   async hide() {
      super.hide();
      this.editing = false;
      this.removeDiv();
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

      if (!draw)
         return;

      if (this.div == undefined)
         this.div = this.createDiv();

      var div = this.div;

      var matrix = context.matrix;
      var dim = this.dimensions.matrixTransform(matrix);
      div.style.left = dim.min.x + "px";
      div.style.top = dim.min.y + "px";
      div.style.width = dim.width + "px";
      div.style.height = dim.height + "px";


      return draw;
   }

   /*
   click(point) {
      this.output = eval("(x, y) => x + y")("bee", "brett");
      alert(this.output);
   }
   */
  
   get editing() {
      return this._editing;
   }

   set editing(value) {
      var div = this.div;
      if (div) {
         if (value) {
            div.style.zIndex = "3";
            div.contentEditable = true;
            div.classList.add("editing");
            div.focus();
            div.innerHTML = div.innerText;
         }
         else {
            div.contentEditable = false;
            div.style.zIndex = "0";
            div.classList.remove("editing");
            if (this._editing) {
               this.html = div.innerText;
               delete this.f;
               this.save();
               div.innerHTML = "";
               div.appendChild(
                  document.createTextNode(this.html)
               );
            }
         }
      }
      this._editing = value;
   }
   
 
}