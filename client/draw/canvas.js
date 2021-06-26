class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _element = null;
   _lastDrawTimestamp = null;
   _points = [];
   _thumbnail;
   children;
   matrix;

   static ELEMENT_ID = "canvas";
   static VIBRATE_TIME = 75; // millisecs
   
   constructor(input) {
      super(getElement(), input);
 
      var canvas = this;
      
      if (input == undefined)
         input = {}
      
      if (input.matrix != undefined) {
         this.matrix =
            Matrix
            .fromJSON(input.matrix);
      }
      else
         this.matrix = new Matrix();
         
      if (input.children != undefined)
      {
         this.children = new Children(...input.children);
      }
      else
         this.children = new Children();
         
      this._thumbnail = new Image();
      this._thumbnail.onload = function() {
         canvas.draw();
      };
      
      this._thumbnail.src = localStorage.getItem(
         "authentication.thumbnail"
      );
      
      var element = getElement();
      
      function getElement() {
         var element =
            document.getElementById(
               Canvas.ELEMENT_ID
            );
      
         if (!element) {
            element =
               document
               .createElement("canvas");
         
            element.id = Canvas.ELEMENT_ID;
   
            setStyle(element);

            document.body.appendChild(
               element
            );
         }
      
         return element;
         
      }
      
      this._resized = false;
      
      setStyle(element);
      
      this._element = element;
      
      this._context = null;
   
      window.addEventListener("resize",
         function() {
            canvas._resized = false;
            canvas.resize(true);
         }
      );
      
      this.resize(false);

      function setStyle(element) {
         
         element.style.position =
            "absolute";
   
         element.style.left = "0px";
         element.style.top = "0px";
         element.style.right = "0px";
         element.style.bottom = "0px";
         element.style.margin = "0";
         element.style.border = "0";
         element.style.zIndex = "1";
         
      }

   }
   
   toJSON() {
      return {
         matrix: this.matrix,
         children: this.children
      }
   }
   
   get context() {
      if (!this._resized)
         this.resize(false);
      
      if (!this._context) {
         this._context =
            this._element.getContext("2d");
         this._context.clipRegion =
            this.dimensions;
      }
      
      this._context.matrix = this.matrix;
      
      return this._context;
   }
   
   draw(forceDraw = false) {
      
      var element = this._element;
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
            canvas.resize(false);

         var context =
            canvas.context;

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
   
   resize(redraw = true) {
         
      console.log("Resize");
      
      var element = this._element;

      // set the canvas _elements
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

      // draw on the canvas
      if (redraw)
         this.draw()

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
            matrix: this.inverse
         }
      );
      
      // Find its smallest parent
      var parent =
         await this.children.findParent(
            line,
            this.matrix.copy()
         );
         
      if (!parent)
         parent = this;
 
      // Find children under parent that
      // are contained by the new line
      var childrenMap =
         await parent.children.findChildren(
            line,
            this.matrix.copy()
         );

      // Remove children from the parent
      // so we can add it under the new line
      var parentLines = 
         parent.children.filter(
            pointer => !childrenMap.has(
               pointer.key
            )
         );
         
      parent.children = new Children(...parentLines);

      
      // Add the children under the new line
      childrenMap.forEach(
         child => line.children.push(
            new Pointer({object: child})
         )
      );
      
      // Add the new line under the parent.
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
   try{
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      console.log("longPress");
       // Create the form
      var form = new Form(
         {
            point,
            matrix: this.inverse
         }
      );
      console.log("form");
      
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
   catch(error) {
      alert(error.stack);
   }
   }
   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      
      var selection = await this.children.hitTest(
         point,
         this.matrix.copy()
      );
      
      if (selection) {
         
         selection.selected = 
           !selection.selected;
           
         this.draw();
         
         selection.save();
      }
      
      
      
      return true;
      
   }
   
   remove() {
      document.body.removeChild(
         this._element
      );
   }
   
   transform(from, to, scale) {

      this.matrix.translateSelf(
         to.x, to.y, 0
      );
      
      this.matrix.scaleSelf(
         scale, scale, 1
      );
      
      this.matrix.translateSelf(
         -from.x, -from.y, 0
      );
     
      this.draw();
   }
   
   screenToCanvas(screenPoint) {
   
      var canvasPoint =
         screenPoint
            .matrixTransform(this.inverse);

      return canvasPoint;
   }
   
   endTouchTransform() {
      this.save();
   }
   
   get inverse() {
      return this.matrix.inverse();
   }
   
   
   static async load()
   {
      var rootId = new Id(
         {
            name: "Canvas",
            ms: 0,
            inc: 0
         }
      );
      
      var key = await
         storage.getItem(rootId.key);
         
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
         storage.setItem(rootId.key, key);
      }
      
      canvas.draw(true);
      
      return canvas;

   }
   

}

CanvasRenderingContext2D
   .prototype
   .applyMatrix =
   function(matrix) {
      this.matrix = matrix;
      this.setTransform(
         matrix.a,
         matrix.b,
         matrix.c,
         matrix.d,
         matrix.e,
         matrix.f
      );
   }
