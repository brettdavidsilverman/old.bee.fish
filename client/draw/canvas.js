class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _lastDrawTimestamp = null;
   _points = [];
   _thumbnail;
   _inverse;
   children;
   matrix;

   static VIBRATE_TIME = 75; // millisecs
   
   constructor(input) {
      super(createElement(), input);

      var canvas = this;
      
      if (input == undefined)
         input = {}
      
      if (input.matrix == undefined) {
         this.matrix = new Matrix();
      }
      else
         this.matrix =
            Matrix.fromJSON(input.matrix);
         
         
      if (input.children == undefined)
         this.children = new Children();
      else
         this.children =
            new Children(...input.children);
         
         
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
         matrix: this.matrix,
         children: this.children
      }
   }
   
   get context() {
      if (!this._resized)
         this.resize();
      
      if (!this._context) {
         this._context =
            this.element.getContext("2d");
      }
      
      this._context.matrix = this.matrix.copy();
      this._context.clipRegion =
         this.dimensions;
            
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
       
            draw();
         }
      );
      
      function draw() {
      
         if (!canvas._resized)
            canvas.resize();

         var context =
            canvas.context;
 
        // context.matrix = canvas.matrix.copy();
         
         context.resetTransform();
   
         context.clearRect(
            0, 0,
            element.width,
            element.height
         );

         canvas.children.draw(context);
         
         if (canvas._thumbnail.complete)
         {
            drawThumbnail(context);
         }
         
         
      }
      
      function drawThumbnail(context) {
      
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


   }
   
   scrollToTop() {
      document.body.scrollTop = 0; // For Safari
      document.documentElement.scrollTop = 0; // For Chrome, Firefox, IE and Opera
   }
   
   penDown(point) {
   
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
   
   movePen(point) {

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
            points: this._points,
            matrix: this.inverse.copy()
         }
      );
      
      // Find its smallest parent
      var parent =
         await this.children.findParent(
            line,
            this.matrix
         );
         
 
      if (!parent)
         parent = this;
 
      // Find children inside parent that
      // are contained by the new line
      var childrenMap =
         await parent.children.findChildren(
            line,
            this.matrix
         );

      
      // Remove children from their parent
      // so we can add it under the new line
      var parentLines = 
         parent.children.filter(
            pointer => !childrenMap.has(
               pointer.key
            )
         );
         
      parent.children = new Children(...parentLines);

      
      // Add the children inside the new line
      childrenMap.forEach(
         child => line.children.push(
            new Pointer({object: child})
         )
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
   /*
   async longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

       // Create the form
      var form = new Form(
         {
            point,
            matrix: this.inverse.copy()
         }
      );

      
      // Find its smallest parent
      var parent =
         await this.children.findParent(
            form,
            this.matrix.copy()
         );
         
      if (!parent)
         parent = this;

      // Add the new form inside the parent.
      var pointer = 
         new Pointer(
            {
               object: form
            }
         );
         
      parent.children.push(pointer);
   
      form.save();
      parent.save();
         
      this.draw();
      
      return true;
   }
   */
   async longPress(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      
      var selection =
         await this.children.hitTest(
            point,
            this.matrix
         );
      
      if (selection) {
         
         selection.selected = 
           !selection.selected;
           
         this.draw();
         
         selection.save();
      }
      
      return true;
      
   }
   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      
      var selection =
         await this.children.hitTest(
            point,
            this.matrix
         );
      
      if (selection && selection.click) {
         
         selection.click(point);
           
         this.draw();
         
      }
      
      return true;
      
   }
   
   remove() {
      document.body.removeChild(
         this.element
      );
   }
   
   transform(matrix) {

      this.matrix.multiplySelf(matrix);

      this._inverse = null;
      
      this.draw();
   }
   
   
   endTouchTransform() {
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
      var userRootId = new Id(
         {
            name: "Root",
            ms: 0,
            inc: 0
         }
      );
      
      var key = await
         storage.getItem(userRootId.key);
         
      var loaded;
      var canvas;
      
      if (key)
      {
         console.log("Fetching canvas");
         loaded = await Id.load(key);
      }
      
      if (loaded)
         canvas = loaded;
      else
      {
         console.log("Creating new canvas");
         canvas = new Canvas();
      
         var key = await canvas.save();
         storage.setItem(userRootId.key, key);
      }
      
      return canvas;

   }
   

}

CanvasRenderingContext2D
   .prototype
   .applyMatrix =
   function(matrix) {
      this.setTransform(
         matrix.a,
         matrix.b,
         matrix.c,
         matrix.d,
         matrix.e,
         matrix.f
      );
   }
