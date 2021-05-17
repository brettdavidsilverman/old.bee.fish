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
   
   
   async draw(context) 
   {
   
      console.log("Drawing.draw.start");
      
      var dimensions = await this.dimensions;
      var contextDimensions = await
         context.dimensions;
         
      if ( !dimensions.intersects(
              contextDimensions) )
      {
         return false;
      }

      await this.drawFrame(context);
      
      // draw child drawings
      var children = await this.children;
      
      console.log(children);
      
      var promises = [];
      
      await children.forEach(
         async function(child)
         {
            if (child)// && child.draw)
            {
              // var promise =
               await child.draw(context);
              // promises.push(promise);
            }
         }
      );
      
    //  Promise.all(promises);
      console.log("Drawing.draw.end");
      
      return true;


   }
   
   async drawFrame(context)
   {
      var parent = await this.parent;
      var layer = await this.layer;
      var selected = await this.selected();
      
      var drawFrame =
          ( selected ||
            (parent == layer) );
      
      if (drawFrame)
      {
         var frame = await this.frame;
         this.setStyle(frame);
         frame.draw(context);
      }
      
      return drawFrame;
   }
   
   setStyle(app) {
      console.log("Error: Drawing Abstract function");
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
   

   async addChild(child, position) {
      child.parent = this;
      child.canvas = await this.canvas;
      child.layer = await this.layer;
      var children = await this.children;
      if (position === "start")
         children.unshift(child);
      else
         children.push(child);
      await this.calculateDimensions(child);
      var dimensions = await this.dimensions;
   }
   
   async calculateDimensions(child) {
      
      var dimensions = await this.dimensions;
      var childDimensions = await
         child.dimensions;

      if (!dimensions)
          dimensions =
             new Dimensions();
               
      dimensions.min = Point.min(
         dimensions.min,
         childDimensions.min
      );
               
      dimensions.max = Point.max(
         dimensions.max,
         childDimensions.max
      );
       
      this.dimensions = dimensions;
      
      var parent = await this.parent;
      if (parent)
         parent.calculateDimensions(this);
   }
   
 
   async hitTest(point, event) {

      var hit = await
         this.search(isPointInside, event);
      
      return hit;
      
      async function isPointInside(object) {
      
         var dimensions =
            await object.dimensions;
            
         var pointIsInside = await dimensions
            .isPointInside(point);
         
         return pointIsInside;
      }
   }
   
   async contains(dimensions) {

      return await this.search(contains);
      
      async function contains(child) {
      
         var childDimensions = await
            child.dimensions;
         
         var childContains =
            childDimensions
            .contains(dimensions);
            
         return childContains;
      }
 
   }
   
   
   async search(condition, event) {
      
      var hit = null;
      var matches = await condition(this);
      
      if ( matches )
      {
    
         hit = this;
         
         var children =
            await this.children;
            
         await children.forEach(
         
            async function (child)
            {
               var testChild;
               
               if (child && child.search)
               {
                  testChild = await
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
  
      if (hit)
      {
         var hitParent = await hit.parent;
         if (!hitParent)
         {
            return null;
         }
      }
      
      return hit;
   }
      
   async selected() {
      var layer = await this.layer;
      var selection = await layer.selection;
      return (selection == this);
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