class App extends Drawing {

   constructor(
      input = {
         statement: "this.promptStatement()",
         childApps: [],
         parent: null,
         layer: null,
         canvas: null,
         lines: [],
         output: undefined
      },
      fromStorage
   ) 
   {
      super(input);

      if (!this.childApps)
         this.childApps = [];
      
      if (this.statement)
         this.createFunction();

      if (!this.min || !this.max) {
         this.setDimensions();
         this.resize();
      }
      
      if (!fromStorage)
         this.setStyle();
         
   }
   
   click(point) {

      if (this.f) {
         this.runFunction();
         return true;
      }
      
      return false;
   }
   
   get matrix() {
      if (this._matrix)
         return this._matrix;
      
      if (this.parent)
         return this.parent.matrix;
         
      return this.canvas.matrix;
   }
   
   set matrix(value) {
      this._matrix = value;
   }
   
   get layer() {
      if (this._layer)
         return this._layer;
      if (this.parent)
         return this.parent.layer;
      return null;
   }
   
   set layer(value) {
      this._layer = value;
   }
   
   draw(context) 
   {
      
      context.save();
      
      if (this.parent)
         super.draw(context);
         
      context.strokeStyle =
         this.strokeStyle;
         
      context.fillStyle =
         this.fillStyle;
         
      context.lineWidth =
            this.lineWidth /
            context.getScale().x;
      
      this.childApps.forEach(
         child =>
            child.draw(context)
      );
      

      
      context.restore();
      
   }
   
   
   setStyle() {
   
      this.strokeStyle = "black";
      if ( this.layer && this ==
           this.layer.selectedApp )
      {
         this.fillStyle ="yellow";
      }
      else if (this.error) {
         this.fillStyle = "red";
      }
      else if (this.f &&
              this.output == undefined)
      {
         this.fillStyle = "orange";
      }
      else if (this.f &&
              this.output != undefined)
      {
         this.fillStyle = "green";
      }
      else
      {
         this.fillStyle = "blue";
      }

   }
   
   longPress(point) {

      
      var selectedApp =
         this.layer
            .selectedApp;
      
      if (selectedApp)
         selectedApp.setStyle();
         
      this.layer
         .selectedApp = this;
      
      if (this == selectedApp) {
      
         if (!this.editor) {
         
            // create the editor
            this.editor =
               new Editor(
                  {
                     selectedApp: this,
                     canvas: this.canvas
                  }
               );
         }
      
         // push the editor on
         // the layers stack
         var layer = new Layer(
            {
               editor: this.editor
            }
         );
         
         this.canvas.addLayer(
            layer
         );

      }
      
      this.setStyle();
      
   }
   
   
   promptStatement() {
      var statement =
         prompt(
            "Statement",
             this.statement
         );
      
      if (statement == null)
         // user cancelled
         return;
         
      this.statement = statement;
      
      this.createFunction();
      
      this.setStyle();
      
      this.save();
      
      canvas.draw();
   }
  
   createFunction() {
   
      delete this.f;
      delete this.error;
      delete this.output;
      
      try {
         this.f = new Function(
            this.statement
         );
      }
      catch (error) {
         this.error = error;
         alert(this.error)
      }
   }
   
   runFunction() {
      delete this.output;
      try {
         this.output = this.f();
         this.canvas.save();
         this.canvas.draw();
      }
      catch (error) {
         this.error = error;
         alert(this.error);
      }
   }
      
   removeChildApp(childApp) {
      // remove the child app
      // from our collection
      this.childApps =
         this.childApps.filter(
            app =>
              app != childApp
         );
      this.setDimensions();
      this.resize();
   }
   
   removeLastLine() {
      super.removeLastLine();
      if (!this.lines.length) {
         this.canvas.selectedApp = null;
         if (this.parent)
            this.parent.removeChildApp(this);
      }
   }
   
   addChildApp(child) {
      child.parent = this;
      child.canvas = this.canvas;
      child.setDimensions();
      this.min =
         Point.min(this.min, child.min);
      this.max =
         Point.max(this.max, child.max);
      this.childApps.push(child)
      this.resize();
   }
   
   setDimensions() {

      super.setDimensions();
      
      var min = this.min;
      var max = this.max;

      this.childApps.forEach(
         child => {
            min = Point.min(
               min, child.min
            );
            max = Point.max(
               max, child.max
            )
         }
      );
      
      this.min = min;
      this.max = max
   }
   

   
   hitTest(point) {

      var hit = null;
      
      if ( this
           .isPointInside(point) )
      {
         hit = this;
         
         this.childApps.forEach(
            function(child) {
               var testHit =
                  child.hitTest(point);
               if (testHit)
                  hit = testHit;
            }
         )
         
      }
         
      return hit;
   }
   
   
   resize() {
      
      super.resize();

      if (this.parent != null)
         this.parent.resize();

   }
   
   hashFilter(key, value) {
      if ( [
           ].includes(key) )
         return undefined;
         
      return super.hashFilter(
         key, value
      );
   }
   
}