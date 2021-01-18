class Drawing {

   _transformMatrix;
   _inverseTransformMatrix;
   _matrix;
   _inverse;
   
   constructor(input) {
   
      Object.assign(this, input);
      
      this["="];
      
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
      
     
   }
   
   
   
   draw(context) 
   {
   
      if (!this.dimensions.intersects(
         context.dimensions)) {
         return false;
      }

      this.drawFrame(context);
      
      //console.log(this.children);
      
      // draw child drawings
      this.children.forEach(
         function(child)
         {
            if (child && child.draw)
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
      console.log("Error: Drawing 73 Abstract function");
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
      if (position === "start")
         this.children.unshift(child);
      else
         this.children.push(child);
      console.log(this.children);
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
         
            function (child)
            {
               var testChild;
               
               if (child && child.search)
               {
                  testChild =
                     child.search(
                        condition,
                        event
                     );
               }
               
               if (testChild)
                  hit = testChild;
            }
         );
         
      }
  
      
      if (hit && !hit.parent) {
         return null;
      }
      
      return hit;
   }
      
   get selected() {
      if (this.layer.selection === this)
         return true;
      else
         return false;
   }
   
   click(point) {
      return false;
   }
   
   // transform matrix
   get transformMatrix() {
      return this._transformMatrix;
   }
   
   set transformMatrix(value) {
      this.copyMatrix(
         value,
         "_transformMatrix"
      );
   }
   
   // inverse transform matrix
   get inverseTransformMatrix() {
      return this._inverseTransformMatrix;
   }
   
   set inverseTransformMatrix(value) {
      this.copyMatrix(
         value,
         "_inverseTransformMatrix"
      );
   }
   
   // matrix
   get matrix() {
      return this._matrix;
   }
   
   set matrix(value) {
      this.copyMatrix(
         value,
         "_matrix"
      );
   }
  
   
   // inverse
   get inverse() {
      return this._inverse;
   }
   
   set inverse(value) {
      this.copyMatrix(value, "_inverse");
   }

   copyMatrix(source, destProperty) {
      if (this[destProperty])
         this[destProperty]
            .fromMatrix(source);
      else
         this[destProperty] =
            Matrix.fromMatrix(source);
   }
}