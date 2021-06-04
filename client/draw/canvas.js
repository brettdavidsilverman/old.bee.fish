class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _element = null;
   _lastDrawTimestamp = null;
   _points = [];
   line;
   
   matrix;
   inverse;
   
   static ELEMENT_ID = "canvas";
   static MAX_MOVE = 18; // Pixels
   static LONG_PRESS_TIME = 300; // millis
   static VIBRATE_TIME = 150; // millisecs
   static ZOOM_INTENSITY = 0.5;
   
   constructor(input) {
      super(getElement(), input);
 
      if (input == undefined)
         input = {}
      
      if (input.matrix != undefined)
         this.matrix =
            new Matrix(input.matrix);
      else
         this.matrix = new Matrix();
         
      this.inverse = this.matrix.inverse();
    
      if (input.line != undefined)
      {
         this.line = new Line(input.line);
      }
      
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
         matrix: this.matrix.toString(),
         line: this.line
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
      var resized = this._resized;
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
      
         if (!resized)
            canvas.resize(false);

         var context =
            canvas.context;

         context.resetTransform();
   
         context.clearRect(
            0, 0,
            element.width,
            element.height
         );

         context.applyMatrix(canvas.matrix);
         
         if (canvas.line != undefined)
         {
            canvas.line.draw(context);
         }
         
      }
      

   }
   
   resize(redraw = true) {
         
      console.log("Resize");
      
      var element = this._element;

      // set the canvas _elements
      // width in pixels
      element.width =
         window.innerWidth *
         window.devicePixelRatio;

      // set width in actual
      // millimeters
      this.width =
         element.width;
    
      // and the height
      // pixels
      element.height = 
         window.innerHeight *
         window.devicePixelRatio;

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
      
      context.strokeStyle = "black";
   
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
      
      if (!this._points) {
         return;
      }
      
      this.line = new Line(
         {points: this._points}
      );

      this.save().then(
         () => console.log("Saved")
      );
      
      this.draw();
      
      return;
      
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
         from.x, from.y, 0
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
      var key = await
         storage.getItem("Canvas.key");
         
      var loaded;
      var canvas;
      
      if (key)
      {
         console.log("Fetching canvas");
         loaded = await Id.load(Canvas, key);
      }
      
      if (loaded)
         canvas = loaded;
      else
      {
         console.log("Creating new canvas");
         canvas = new Canvas();
      
         var key = await canvas.save();
         storage.setItem("Canvas.key", key);
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
