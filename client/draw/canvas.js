class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _element = null;
   _lastDrawTimestamp = null;
   _points = [];
   _line;
   
   static ELEMENT_ID = "canvas";
   static MAX_MOVE = 18; // Pixels
   static LONG_PRESS_TIME = 300; // millis
   static VIBRATE_TIME = 150; // millisecs
   static ZOOM_INTENSITY = 0.5;
   
   constructor(input) {
      super(getElement(), input);
 
      if (!input)
         input = {}

      if (input._line != undefined)
         this._line = new Line(input._line);
         
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
      
      this.resize(true);

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
         time: this.time,
         increment: this.increment,
         _line: this._line
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
   
   async draw(forceDraw = false) {
      
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

         if (canvas._line != undefined)
            canvas._line.draw(context);
         
      }
      

   }
   
   resize(redraw = true) {
         
      var element = this._element;
      
      // set the canvas _elements
      // width in pixels
      element.width =
         window.innerWidth *
         this.devicePixelRatio;

      // set width in actual
      // millimeters
      this.width =
         element.width;
    
      // and the height
      
      // pixels
      element.height = 
         window.innerHeight *
         this.devicePixelRatio;

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
   
   async penUp() {
      
      
      if (!this._points) {
         return;
      }
      
      var canvas = this;
      
      var line =
         new Line({points: this._points});

      console.log(line);
      
      this._line = line;
      
      this._line.save();
      
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
   
   endTouchTransform() {
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
   .getScale = 
function() {


   var pixPerMm =
      window.getPixelsPerMillimeter();
   
   var scaleVector = {
      x: this.matrix.m11 / pixPerMm.x,
      y: -this.matrix.m22 / pixPerMm.y
   }

   var scale =
      (scaleVector.x + scaleVector.y) / 2;
   
   return scale;
}

Object.defineProperty(
   CanvasRenderingContext2D
   .prototype,
   "_scale",
   {
      get: CanvasRenderingContext2D
   .prototype.getScale
   }
);

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
