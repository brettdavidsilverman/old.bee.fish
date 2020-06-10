class Code {
   #app
   #pre
   #textNode
   app;
   constructor(input) {
   
      console.log("Code()");
      
      this.app = input.app;
      var code = this;
      this.#pre =
         document.createElement("pre");
         
      this.#textNode =
         document.createTextNode(
            this.app.statement
         );
         
      this.#pre.appendChild(
         this.#textNode
      );
      
      this.#pre.style.position = "absolute";
      this.#pre.style.opacity = "0.5";
      
      
      this.#pre.style.margin = "0";
      this.#pre.style.border = "thick solid #000000";
      var dim = this.app.dimensions;
      this.#pre.style.top =
         dim.max.y + "px";
      this.#pre.style.left =
         dim.min.x + "px";
      this.#pre.style.width =
         dim.width + "px";
      this.#pre.style.height =
         dim.height + "px";
      
      this.#pre.style.zIndex = "2";
      this.#pre.contentEditable = true;
      
      document.body.appendChild(
         this.#pre
      );
   }
  
   setTransform(matrix) {
      this.#pre.style.transform =
         matrix.toCSS();
   }
   
   remove() {
      this.#pre.remove();
   }
   
   save() {
      this.app.statement =
         this.#pre.innerText;
   }
   
   draw(context) {
      var scale = context._scale;
      var fontSize = 3 / scale; //mm
      
      this.#pre.style.fontSize =
         String(fontSize) + "mm";
      console.log(fontSize);
      this.setTransform(context.matrix);
   }
}