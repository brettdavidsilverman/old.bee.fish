class Editor extends App {

   constructor(
      input = {
         selectedApp: null,
         canvas: canvas
      }
   ) {
      super(input);
      
      var editor = this;
      
      if (!this.setStatementApp) {
    
         this.setStatementApp =
            new Button(
               {
                  statement: "this.selectedApp.promptStatement()",
                  width: 10,
                  height: 10,
                  topLeft: new Point(
                    {
                       x: - this.canvas
                            .width / 2,
                       y:   this.canvas
                            .height / 2
                    }
                  ),
                  lineWidth: 0.5,
                  parent: this,
                  selectedApp:
                     this.selectedApp
               }
            );
         this.addChildApp(
            this.setStatementApp
         );
         
      }
      
   
   }
   
}
   
class Button extends App {
   constructor(
      input = {
         label: null,
         topLeft: null,
         width: 10,
         height: 10,
         lineWidth: 0.5,
         selectedApp: null
      }
   )
   {
      super(input);
   
      if (!this.min || !this.max)
         this.setDimensions();
      
   }
   
   draw(context) {
      super.draw(context);
   }
   
   setDimensions() {
      super.setDimensions()
      
      var min = new Point(
         {
            x: this.topLeft.x,
            y: this.topLeft.y -
               this.height
         }
      );
      
       var max = new Point(
         {
            x: this.topLeft.x +
               this.width,
            y: this.topLeft.y
         }
      );
      
      this.min = Point.min(min, this.min);
      this.max = Point.max(max, this.max);
   }

}