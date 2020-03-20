function CodeEditor(input) {

   var codeEditor = this;
   var originalCode;
   
   if (input && input.id)
      this.id = input.id;
      
   var element;      
 
   var UIEvents = input.UIEvents;
   var canvas = input.canvas;
   var drawing = input.drawing;
   
   var buttons = {
      ok: null,
      cancel: null
   }
   
   this.resize = function() {
      Style.applyToElement(
         {
            style: Style.codeEditor,
            element: element
         }
      );
   }
   
   canvas.onresize = function() { 
      codeEditor.resize();
   }
   
   load();
   
   this.onok = function() {}
   this.oncancel = function() {}

   this.ok = function() {
      
      if (!drawing.loadCode(
            element.innerText
         )) {
         alert(drawing.error);
         element.focus();
         return false;
      }
      
      unload();
      
      this.onok();
      canvas.save();
   }
   
   this.cancel = function() {
   
      drawing.loadCode(originalCode);
      
      unload();
      
      this.oncancel();
      
      canvas.draw();
   }
  
   /*
   var uiEvents = new UIEvents(
      {
         element: element,
         pixelRatio: canvas.pixelRatio
      }
   );

   uiEvents.ontransform = function(scale) {
      // 4.25vw
      element.style.fontSize = (4.25 * scale) + "vw";
   }
   */
   
   this.draw = function(context) {
   }
   
 
   function load() {
      originalCode = drawing.code;
      
      element =
         document
         .createElement("code");
         
      element.id = codeEditor.id;
      
      if (!isNull(drawing.code))
         element.innerText =
            String(drawing.code);
      else
         element.innerText = "";
         
      element.style.display = "block";
      element.contentEditable = true;
      canvas.getElement().parentNode.appendChild(
         element
      );
      
      buttons.ok =
         canvas.loadButton("ok", canvas);
      buttons.cancel =
         canvas.loadButton("cancel", canvas);
      buttons.ok.onclick =
         function() {
            codeEditor.ok();
         }
  
      buttons.cancel.onclick =
         function() {
            codeEditor.cancel();
         }
         
      element.focus();
      
      codeEditor.resize();
      
      canvas.draw();
      
   }
   
   function unload() {
      canvas.removeButton(buttons.ok);
      canvas.removeButton(buttons.cancel);
      element.parentNode.removeChild(
         element
      );
      canvas.draw();
   }
}

