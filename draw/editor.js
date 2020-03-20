function Editor (input) {
   App.call(this, input);
         
   var editor = this;
   
   if (!input.canvas)
      this.canvas = canvas;
      
   this.createButtons = function() {
   
      var buttons = [
         // Set statement
         new Button(
            {
               parent: this,
               label: "f",
               statement: "selection.promptStatement()",
               width: 10,
               height: 10,
               topLeft: this.canvas.topLeft,
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Remove
         new Button(
            {
               parent: this,
               label: "x",
               statement: "selection.confirmRemove()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 20,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black",
            }
         ),
         // Prompt Label
         new Button(
            {
               parent: this,
               label: "a",
               statement: "selection.promptLabel()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 40,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Prompt text
         new Button(
            {
               parent: this,
               label: "T",
               statement: "selection.promptText()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 60,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Prompt console
         new Button(
            {
               parent: this,
               label: "c",
               statement: "selection.promptConsole()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 80,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         )
      ];
      
      this.children = [];
      buttons.forEach(
         function(button) {
            editor.addChild(button);
         }
      );

      this.createdButtons = true;
   }
   
   if (input.createdButtons == null)
      this.createButtons();
}
   
function Button(input) {

   if (input.dimensions == null)
      input.dimensions = 
      Dimensions.fromRectangle(input);
      
   if (input.canvas == null)
      input.canvas = input.parent.canvas;
      
   if (input.layer == null)
      input.layer = input.parent;
      
   App.call(this, input);
   
   this.drawFrame = function(context) {
      this.setStyle(this.frame);
      this.frame.draw(context);
   }

}