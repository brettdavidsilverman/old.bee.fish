class Drawing {

   #matrix;
   
   constructor(input) {
   
      Object.assign(this, input);
   
      if (!input.canvas)
         this.canvas = canvas;
      
      if (!input.children)
         this.children = [];

      if (!input.dimensions)
         this.dimensions = new Dimensions();
      
      if (!input.frame) {
         this.frame = new Rectangle(
            {
               parent: this,
               dimensions: this.dimensions,
               lineWidth: 0.5,
               strokeStyle: "yellow",
               fillStyle: null
            }
         );
      }
     
      if (canvas)
         this.selection = this.search(
            (drawing) => 
               drawing == canvas.selection
         );
      else
         this.selection = this;
   }
   
   get matrix() {
      if (this.#matrix)
         return this.#matrix;
      
      if (this.parent)
         return this.parent.matrix;
 
      return this.canvas.matrix;
   }
   
   set matrix(value) {
      this.#matrix = value;
   }
   
   draw(context) 
   {

      if (this.dimensions.intersects(
         context.dimensions) == false) {
         return false;
      }

      this.drawFrame(context);
         
      // draw child drawings
      this.children.forEach(
         function(child) {
            child.draw(context);
         }
      );
      
      return true;


   }
   
   drawFrame(context) {
      var drawFrame =
         (this.selected ||
            (this.parent == this.layer));
      if (drawFrame) {
         this.setStyle(this.frame);
         this.frame.draw(context);
      }
      return drawFrame;
   }
   
   setStyle(app) {
      console.log("Error: Drawing 84 Abstract function");
   }
 
   confirmRemove() {
   
      var label = this.label;
      if (label == null)
         label = this.name;
         
      var confirmed =
         confirm("Remove " + label + "?");
         
      if (confirmed)
         this.remove();
   }
   
   remove() {
      
      // Get the index of the child drawing
      // to remove
      var parent = this.parent;
      var length = parent.removeChild(this);
      if (parent == this.layer)
         this.canvas.layers.pop();
      else {
      
         // recalculate dimensions
         if (length == 0)
            canvas.selection = parent;
         else
            canvas.selection =
               parent.children[
                  length - 1
               ];
         this.dimensions = new Dimensions();
         this.calculateDimensions();
      }
      
      this.canvas.draw();
      
   }
   
   // remove the child
   // from our collection
   removeChild(child) {
      var index =
         this.children.indexOf(child);
      this.children.splice(index, 1);
      return this.children.length;
   }
   

   addChild(child, position) {
      child.parent = this;
      child.canvas = this.canvas;
      child.layer = this.layer;
      if (position == "start")
         this.children.unshift(child);
      else
         this.children.push(child);
         
      this.calculateDimensions(child);

   }
   
   calculateDimensions(child) {
      
      if (!this.dimensions)
          this.dimensions =
             new Dimensions();
               
      this.dimensions.min = Point.min(
         this.dimensions.min,
         child.dimensions.min
      );
               
      this.dimensions.max = Point.max(
         this.dimensions.max,
         child.dimensions.max
      );
       
      if (this.parent)
         this.parent.calculateDimensions(this);
   }
   
 
   hitTest(point, event) {

      var hit = this.search(isPointInside, event);
      
      return hit;
      
      function isPointInside(object) {
      
         var pointIsInside =
            object
            .dimensions
            .isPointInside(point);

         return pointIsInside;
      }
   }
   
   contains(dimensions) {

      return this.search(contains);
      
      function contains(child) {
         var childContains =
            child
            .dimensions
            .contains(dimensions);
         return childContains;
      }
 
   }
   
   
   search(condition, event) {
      
      var hit = null;
      
      if ( condition(this) )
      {
    
         hit = this;
         
         this.children.forEach(
            function(child) {
               var testChild = 
                  child.search(
                     condition,
                     event
                  );
               if (testChild)
                  hit = testChild;
            }
         )
         
      }
  
      
      if (hit && !hit.parent) {
         return null;
      }
      
      var name = "null";
      if (hit)
         name = hit.name;
      
      return hit;
   }
      
   get selected() {
      var selected =
         (canvas.selection === this);
      return selected;
   }

}