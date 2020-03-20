function Text(element) {
   
   this.parent = element;
   
   this.element = document.createElement("code");
      
   this.element.contentEditable = "true";
   
   this.element.innerHTML = 'alert("hello world");';
  
   var dimensions = new Dimensions(.get
    
   dimensions.applyTo(this.element);
   
   this.element.style.position = "fixed";
   this.element.style.overflow = "auto";
   
   this.parent.appendChild(this.element);

}