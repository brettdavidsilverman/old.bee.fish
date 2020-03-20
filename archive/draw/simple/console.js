class Console {

   constructor() {
   
      var console = this;

      this.element =
         document.createElement(
            "code"
         );
         
      this.element.id = "console";
      
      setStyle(this.element);
      
      document.body.appendChild(
         this.element
      );
      
      function setStyle(element) {
      
         element.style.position =
            "absolute";
         element.style.top = "0px";
         element.style.left = "0px";
         element.style.right = "0px";
         element.style.color = "red";
         element.style.wordWrap =
            "break-word";
         element.style.whiteSpace =
            "pre-wrap";
      }
   }

   
   log(obj) {
      var text =
         this.element.innerHTML;
      var string;
      var objects = [];
      if (obj == null)
         string = "null";
      else if (obj == undefined)
         string = "undefined";
      else if (typeof obj == "string")
         string = obj
      else if (typeof obj == "object")
         string = obj.toString();
      else
         string = String(obj);
      
      text =
         string +
         "<br/>" +
         text.substr(0, 1000);
          
      this.element.innerHTML = text;


   }
   
}