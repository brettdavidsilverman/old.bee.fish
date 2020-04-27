function Drawing(input) {
   var drawing = this;
   
   Object.assign(this, input);
   
   if (input.canvas == null)
      this.canvas = canvas;
      
   if (input.children == null)
      this.children = [];

   if (!input.dimensions)
      this.dimensions = new Dimensions();
      
   if (input.frame == null) {
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

   this.getMatrix = function() {
      if (this._matrix)
         return this._matrix;
      
      if (this.parent)
         return this.parent.matrix;
 
      return this.canvas.matrix;
   }
   
   this.setMatrix = function(value) {
      this._matrix = value;
   }
   
   Object.defineProperty(
      this,
      "matrix",
      {
         get: this.getMatrix,
         set: this.setMatrix
      }
   );
   
   this.draw = function(context) 
   {
      console.log("Draw: " + this.constructor.name);
      

      if (this.dimensions.intersects(
         context.dimensions) == false) {
         console.log("...skip: " + {_this:this.dimensions, con:context.dimensions});
         return false;
      }

      
      this.setStyle(context);
      

      
      // top level draws frame
      // around all its children
      this.drawFrame(context);
         
      // draw child drawings
      this.children.forEach(
         function(child) {
            child.draw(context);
         }
      );
      
      return true;


   }
   
   this.drawFrame = function(context) {
      if (this.selected) {
         this.frame.draw(context);
      }
   }
   
   this.setStyle = function(context) {
   }
   
   this.confirmRemove = function() {
   
      var label = this.label;
      if (label == null)
         label = this.name;
         
      var confirmed =
         confirm("Remove " + label + "?");
         
      if (confirmed)
         this.remove();
   }
   
   this.remove = function() {
      
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
   this.removeChild = function(child) {
      var index =
         this.children.indexOf(child);
      this.children.splice(index, 1);
      return this.children.length;
   }
   

   this.addChild = function(child, position) {
      child.parent = this;
      child.canvas = this.canvas;
      child.layer = this.layer;
      if (position == "start")
         this.children.unshift(child);
      else
         this.children.push(child);
         
      this.calculateDimensions(child);
      
   }
   
   this.calculateDimensions = function(child) {
      
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
   
 
   this.hitTest = function(point, event) {

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
   
   this.contains = function(dimensions) {

      return this.search(contains);
      
      function contains(child) {
         var childContains =
            child
            .dimensions
            .contains(dimensions);
         return childContains;
      }
 
   }
   
   
   this.search = function(condition, event) {
      
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
         
      console.log("Hit: " + name);
      
      return hit;
   }

   if (canvas)
      this.selection = this.search(
         (drawing) => 
            drawing == canvas.selection
      );
   else
      this.selection = this;
      
   function selected() {
      var selected =
         (canvas.selection === this);
      return selected;
   }
   
   Object.defineProperty(
      this,
      "selected",
      {
         get: selected
      }
   );
   

   
   
}