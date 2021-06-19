class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _element = null;
   _lastDrawTimestamp = null;
   _points = [];
   _thumbnail;
   lines;
   
   matrix;
   inverse;
   
   static ELEMENT_ID = "canvas";
   static MAX_MOVE = 18; // Pixels
   static LONG_PRESS_TIME = 300; // millis
   static VIBRATE_TIME = 150; // millisecs
   static ZOOM_INTENSITY = 0.5;
   
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
         
      this.inverse = this.matrix.inverse();
    
      if (input.lines != undefined)
      {
         this.lines = input.lines.map(
            key => new Pointer({key})
         );
      }
      else
         this.lines = [];
         
      this._thumbnail = new Image();
      this._thumbnail.onload = function() {
         canvas.draw();
      };
      
      this._thumbnail.src = sessionStorage.getItem(
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
         lines: this.lines
      }
   }
   
   get context() {
      if (!this._resized)
         this.resize(false);
      
      if (!this._context)
         this._context =
            this._element.getContext("2d");
            
      return this._context;
   }
   
   draw(forceDraw = false) {
      
      var element = this._element;
      var canvas = this;
      
      if (forceDraw) {
         _draw();
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
       
            _draw();
         }
      );
      
      function _draw() {
      
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

         if (canvas._thumbnail.complete)
         {
            drawThumbnail(context);
         }
         
         
         
         
         canvas.lines.forEach(
            pointer => {
               pointer.fetch()
               .then(
                  line => {
                     line.draw(
                        context,
                        canvas.matrix.copy()
                     );
                  }
               );
            }
         );
         
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
   
   penUp() {
      var canvas = this;
      
      if (!this._points) {
         return;
      }
      
      var line = new Line(
         {points: this._points}
      );
      
      line.matrix =
         Matrix.fromMatrix(
            this.inverse
         );
         
      var pointer = 
         new Pointer(
            {
               object: line
            }
         );
         
      this.lines.push(pointer);
      
      line.save();
      
      this.save().then(
         (key) => {
            console.log("Saved")
         }
      );
      
      this._points = null;
      
   }
   
   click(point) {
         
      return true;
   }
   
   longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      
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
     
      this.inverse = this.matrix.inverse();
      
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
