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

   static VIBRATE_TIME = 75; // millisecs
   
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
      
         var element =
            document
            .createElement("canvas");
         
         element.style.position =
            "absolute";
   
         element.style.left = "0px";
         element.style.top = "0px";
         element.style.right = "0px";
         element.style.bottom = "0px";
         element.style.margin = "0";
         element.style.border = "0";
         element.style.zIndex = "1";
         
         document.body.appendChild(
            element
         );
      
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
         ms: super.ms,
         inc: super.inc,
         matrix: this.matrix,
         children: this.children
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
            this.matrix = matrix;
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
         }

         context.pushMatrix(new Matrix());

         // Cache the context
         this._context = context;
      
      }

      return this._context;
   }
   
   draw(forceDraw = false) {
      
      var element = this.element;
      var canvas = this;
      
      if (forceDraw === true) {
         draw();
         return;
      }
      
      window.requestAnimationFrame(
   
         function(timestamp) {
               
            if ( timestamp <=
                 canvas._lastDrawTimestamp
               )
            return;
         
            canvas._lastDrawTimestamp =
               timestamp;
       
            draw().catch(error => canvas.handleError(error));
         }
      );
      
      async function draw() {
      
         if (!canvas._resized)
            canvas.resize();

         var context =
            canvas.context;
         
            context.save();

         context.resetTransform();

         context.clearRect(
            canvas.dimensions.min.x,
            canvas.dimensions.min.y,
            canvas.dimensions.width,
            canvas.dimensions.height
         );

         // Push the first matrix on the matrix
         context.pushMatrix(canvas.matrix);

         await canvas.children.draw(context);

         context.popMatrix();

         if (canvas._thumbnail.complete) {
            await drawThumbnail(context);
         }

         context.restore();

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
         
         context.globalAlpha = 1;

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
      var matrix = this.matrix;
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
         
      this.draw();
      
      this._points = null;
      
      
   }

   async longPress(point) {

      point = this.screenToCanvas(point);

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      if (this.selection) {
         this.selection.selected = false;
         this.selection = null;
      }
   
      var selection =
         await this.children.hitTest(
            point
         );
      
      if (selection) {
         
         this.selection = selection;

         this.selection.selected = true; 
           
         if (!this.toolbox)
            this.toolbox = new Toolbox(
               {
                  canvas: this
               }
            );
         
      }
      else {
         if(this.toolbox) {
            this.toolbox.remove();
            this.toolbox = null;
         }
      }
      
      return this.draw();

      
   }
   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      point = this.screenToCanvas(point);
      
      var selection;

      if (this.toolbox)
         selection = await this.toolbox.hitTest(point);

      if (selection == null) {
         selection =
            await this.children.hitTest(
               point
            );
      }
      
      if (selection && selection.click) {
         
         await selection.click(point);

         if  (this.selection == null && this.toolbox)
            this.toolbox = null;

         this.draw();
         
      }
      
      return true;
      
   }
   
   remove() {
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

   
   static async load()
   {
      var key = await
         storage.getItem("Canvas");
         
      var canvas;
      
      if (key)
      {
         console.log("Fetching canvas");
         var id = Id.fromKey(key);
         canvas = await id.load();
      }
      
      if (canvas == undefined)
      {
         console.log("Creating new canvas");
         canvas = new Canvas();
      
         var id = await canvas.save();
         storage.setItem("Canvas", id.key);
      }
      
      return canvas;

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


