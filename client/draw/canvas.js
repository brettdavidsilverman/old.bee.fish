class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _lastDrawTimestamp = null;
   _points = [];
   _thumbnail;
   _inverse;
   _context = null;
   index;
   children;
   matrix;
   toolbox;
   selection;

   static VIBRATE_TIME = 50; // millisecs
   
   constructor(input) {
      super(input, createElement());

      var canvas = this;
      
      this.index = 0;

      if (input == undefined)
         input = {}
      
      if (input.matrix == undefined) {
         this.matrix = new Matrix();
      }
      else
         this.matrix =
            Matrix.fromJSON(input.matrix);
         

      if (input.children == undefined)
         this.children = new Children(this);
      else {
         this.children =
            new Children(this, ...input.children);
      }         
         
      this._thumbnail = new Image();
      this._thumbnail.onload = function() {
         canvas.draw();
      };
      
      this._thumbnail.src = localStorage.getItem(
         "authentication.thumbnail"
      );
      
      function createElement() {
      
         var element;
         
         element = document.getElementById("canvas");
         if (element == undefined) {
            element = document.createElement("canvas");
            element.id = "canvas";
            document.body.appendChild(
               element
            );
         }
  

         element.style.position =
            "absolute";
   
         element.style.left = "0px";
         element.style.top = "0px";
         element.style.right = "0px";
         element.style.bottom = "0px";
         element.style.margin = "0";
         element.style.border = "0";
         element.style.zIndex = "1";
         
      
         return element;
      }
      
      this._resized = false;
      
      this._context = null;
   
      window.addEventListener("resize",
         function() {
            canvas._resized = false;
            canvas.resize();
            canvas.draw();
         }
      );
      
      this.resize()
   }
   
   toJSON() {
      return {
         ms: this.ms,
         inc: this.inc,
         matrix: this.matrix,
         children: this.children.toJSON()
      }
   }

   get context() {

      // Resize if required
      if (!this._resized)
         this.resize();
      
      // Create context if required
      if (!this._context) {

         var context = this.element.getContext("2d");

         context.stack = [];

         context.pushMatrix = function(matrix) {
            this.save();
            this.stack.push(this.matrix);
            this.applyMatrix(matrix);
         }
         
         context.popMatrix = function() {
            this.matrix = this.stack.pop();
            this.restore();
         }

         // Add Push and Pop functions to the 2d context
         context.applyMatrix = function(matrix) {
            this.setTransform(
               matrix.a,
               matrix.b,
               matrix.c,
               matrix.d,
               matrix.e,
               matrix.f
            );
            this.matrix = matrix;
         }

         context.pushMatrix(new Matrix());

         // Cache the context
         this._context = context;
      
      }

      return this._context;
   }
   
   async draw() {
      
      var element = this.element;
      
      if (!this._resized)
         this.resize();

      var context =
         this.context;
      
      context.save();

         var context =
            canvas.context;
         
         context.resetTransform();

         context.clearRect(
            0,
            0,
            canvas.width,
            canvas.height
         );

         context.dimensions = canvas.dimensions.matrixTransform(canvas.inverse);

         // Push the first matrix on the context stack
         context.pushMatrix(canvas.matrix);

         // Draw our children
         await canvas.children.draw(context);

         // Pop the matrix off the stack
         context.popMatrix();

         if (canvas._thumbnail.complete) {
            // Draw the thumbnail
            await drawThumbnail(context);
         }

      }
      
      async function drawThumbnail(context) {
   
         context.pushMatrix(new Matrix());

         var thumbnail = canvas._thumbnail;
         
         var width = thumbnail.width;
         var height = thumbnail.height;
       
         var destination = new Point(
            {
               x: element.width -
                  width,
               y: 0
            }
         );
         
         context.globalAlpha = 0.5;

         context.drawImage(
            thumbnail,
            0,
            0,
            width,
            height,
            destination.x,
            destination.y,
            width,
            height
         );

         context.popMatrix();
      }

   }
   
   resize() {
        
      var element = this.element;

      // set the canvas elements
      // width in pixels
      element.width =
         window.innerWidth;

      // set width in actual
      // millimeters
      this.width =
         element.width;
    
      // and the height
      // pixels
      element.height = 
         window.innerHeight;

      // millimeters
      this.height =
         element.height;
      
      this.dimensions = new Dimensions(
         {
            min: new Point(
               {
                  x: 0,
                  y: 0
               }
            ),
            max: new Point(
               {
                  x: this.width,
                  y: this.height
               }
            )
         }
      );
      
      // reset the context
      this._context = null;
      
      this._resized = true;
      
      this.scrollToTop();

      //this.children.resize(this);

   }
   
   scrollToTop() {
      document.body.scrollTop = 0; // For Safari
      document.documentElement.scrollTop = 0; // For Chrome, Firefox, IE and Opera
   }
   
   async penDown(point) {
   
      var context = this.context;
      
      context.resetTransform();
      context.lineWidth = 0.3;
      
      context.strokeStyle = "blue";
   
      context.beginPath();
   
      context.moveTo(
         point.x,
         point.y
      );
      
      this._points = [point];

   }
   
   async movePen(point) {

      this._points.push(point);
   
      var context = this.context;
      
      context.lineTo(
         point.x,
         point.y
      );
   
      context.stroke();
   }
   
   async penUp() {

      if (!this._points) {
         return;
      }
      
      // Create the line
      var line = new Line(
         {
            parent: this,
            points: this._points.map(
               point => this.screenToCanvas(point)
            )
         }
      );

      
      // Find its smallest parent
      var parent =
         await this.children.findParent(
            line
         );
         
      if (!parent)
         parent = this;
 
      line.parent = parent;

      // Find children inside parent that
      // are contained by the new line
      var childrenMap =
         await parent.children.findChildren(
            line
         );

      
      // Remove children from their parent
      // so we can add it under the new line
      parent.children.forEach(
         (pointer, index, array) => {
            if (pointer && childrenMap.has(pointer.key))
               array[index] = undefined;
         }
      );
      
      // Add the children inside the new line
      childrenMap.forEach(
         child => {
            child.parent = line;
            line.children.push(
               new Pointer({object: child})
            );
         }
      );
      
      // Add the new line inside the parent.
      var pointer = 
         new Pointer(
            {
               object: line
            }
         );
         
      parent.children.push(pointer);
            
      // Save and draw.
      line.save();
      parent.save();

      this._points = null;
      
      this.draw();
      
      
   }

   
   async save() {
      return super.save();
   }

   async longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      if (this.selection) {
         this.selection.selected = false;
         this.selection = null;
      }
   
      var selection =
         await this.hitTest(
            point
         );
      
      if (selection) {
         
         this.selection = selection;

         this.selection.selected = true; 
           
         this.toolbox = new Toolbox({parent: this});   
      }
      
      return this.draw();

      
   }
   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var selection  = await this.hitTest(point);
      
      if (selection && selection.click) {
         
         await selection.click(point);
         
      }

      this.draw();

      return true;
      
   }

   async hitTest(point) {
      
      var canvasPoint = this.screenToCanvas(point);
      
      return await this.children.hitTest(canvasPoint);
   }
   
   remove() {
      if (window.stack.top === this)
         window.stack.pop();

      if (window.stack.length == 0)
         document.body.removeChild(
            this.element
         );
   }
   
   async transform(matrix) {

      this.matrix.multiplySelf(matrix);

      this._inverse = null;
      
      this.draw();
   }
   
   
   async endTransform() {
      this.save();
   }
   
   get inverse() {
   
      if (this._inverse)
         return this._inverse;
         
      this._inverse =
         this.matrix.inverse();
         
      return this._inverse;
   }

   
   toString() {
      return JSON.stringify(
         this,
         null,
         "   "
      );
   }

   screenToCanvas(point) {
      return point.matrixTransform(this.inverse);
   }

   canvasToScreen(point) {
      return point.matrixTransform(this.matrix);
   }

}


