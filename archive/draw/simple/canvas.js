class Canvas extends Storable {

   constructor(
      input = {
         layers: null
      }
   )
   {
      super(input);
      this.id.key = "Canvas";

      var canvas = this;
      
      this._element =
         document
         .createElement("canvas");

      setStyle(this._element);
      setEvents(this._element);
      
      document.body.appendChild(
         this._element
      );
            
      this._resized = false;
     
      var longPressTimer = null;
      var longPressPoint = null;
      this._penMoved = false;
      this._startPoint = null;
      
      window.onresize = function() {
         canvas._resized = false;
         canvas.resize(true);
      }
      
      this.resize(false);
 
      if (!input.layers) {

         this.layers = new Layers(
            {
               canvas: this
            }
         );
      }
      
      this.draw();
      
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
      
      function setEvents(element) {
         element.ontouchstart = 
         element.onmousedown  =
            onstart;

         element.ontouchmove =
         element.onmousemove =
            onmove;
         
         element.ontouchend    =
         element.ontouchcancel =
         element.onmouseup     =
         element.onmouseleave  =
            onend;
            
         element.onwheel =
            onmousezoom;
      }
      
      function onstart(event) {

         event.preventDefault();
         
         var touchCount = 1;
         
         if ("touches" in event)
            touchCount =
               event.touches.length;
         
         if (touchCount > 1)
         {
            // two or more fingers,
            // start scale/translate
            // mode
            canvas.touchPoints =
               [
                  getPoint(event, 0),
                  getPoint(event, 1)
               ];
            
            // cancel any selected 
            // drawings
            canvas._points = null;
            
            // cancel long press timer
            clearLongPressTimer(
               "multiple"
            );
            
            return;
         }
         
         
         // cancel selected transform
         canvas.touchPoints = null;

         // get the point and start
         // drawing
         var point = getPoint(
            event, 0
         );
   
         this._startPoint = point.copy();
         this._penMoved = false;
         
         canvas.penDown(point);
        
         // Start the timer for long
         // touch
         longPressTimer =
            window.setTimeout(
               onlongpress,
               Canvas.LONG_PRESS_TIME
            );
         longPressPoint =
            this._startPoint.copy();

      }
      
      function onmove(event) {

         event.preventDefault();
         
         if (canvas.touchPoints) {
   

           // get the next two points
           // for translate/scale
           canvas.touchPoints.push(
              getPoint(event, 0)
           );
         
           canvas.touchPoints.push(
              getPoint(event, 1)
           );

           // perform the transform
           canvas.touchTransform();
      
           // shift the two points
           // to replace the first ones
           canvas.touchPoints
              .shift();
             
           canvas.touchPoints
              .shift();
      
         }
         else if (canvas._points) {
      
            // get the next point
            // and move the pen to
            // there
            var point =
               getPoint(event, 0);
               
            canvas.movePen(point);
            
            // If we have moved greater
            // then 5 pix then
            // cancel the long press
            // timer
            if ( !this._penMoved &&
                 Point.distance(
                    point,
                    this._startPoint
                 ) > Canvas.MAX_MOVE ) {
            
               clearLongPressTimer(
                  "move"
               );
               this._penMoved = true;
            }
            
   
         }
      }
      
      function onend(event) {
      
         clearLongPressTimer("end");
         
         if (canvas.touchPoints)
            // stop transforming
            canvas.touchPoints =
               null;
         else if (canvas._points)
         {
            
            var clicked = false;
            if (!this._penMoved) {
               clicked =
                  canvas.click(
                     this._startPoint
                  );
            }
            
            if (!clicked)
               canvas.penUp();
            
            // stop drawing
            canvas._points = null;
         }
         
      }
      
      // mouse wheel used for zooming
      function onmousezoom(event) {
      
         event.preventDefault();
         
         var point = getPoint(event);
         
         canvas
         .transformScreenToCanvas(
            point
         );
         
         // Normalize wheel to +1 or -1
         var wheel = event.deltaY < 0 ?
            1 : -1;
         
         // Compute zoom factor
         var zoom = Math.exp(
            wheel *
               canvas.ZOOM_INTENSITY
         );
         
         canvas.transform(
            point,
            point,
            zoom
         );
      }
      
      
      // a long press has occurred
      function onlongpress(event) {

         // reset the timer
         longPressTimer = null;
         
         // cancel the selected line
         canvas._points = null;
 
         canvas.longPress(
            longPressPoint
         );
      }
      
      // Clear the timeout for long
      // press.
      function clearLongPressTimer(
         reason
      )
      {
         
         if (longPressTimer) {
            window.clearTimeout(
               longPressTimer
            );
            longPressTimer = null;
         }
         
      }
      
      function getPoint(
         event,
         touchIndex)
      {

         var x, y;
   
         if ("touches" in event) {
            var touch =
               event
               .touches[touchIndex];
               
            x = touch.clientX,
            y = touch.clientY
         }
         else {
            x = event.clientX,
            y = event.clientY;
         }
   
         return new Point({x, y});
      }

   }
   
   static load() {
      
      var canvas = Object.load(
         {
            key: "Canvas"
         }
      );

      if (!canvas) {
         canvas = new Canvas();
         canvas.save();
      }
     
      return canvas;
   }
   
   get matrix() {
      if (!this._matrix) {
         var matrix = Array(6);
         glMatrix.mat2d.identity(
            matrix
         );
         this.matrix = matrix;
      }
      return this._matrix;
   }
   
   set matrix(value) {
      this._matrix = value;
      
      if (!this.inverse)
         this.inverse = Array(6);
         
      glMatrix.mat2d
         .invert(
            this.inverse,
            this._matrix
         );
   }
   
   get context() {
      if (!this._resized)
         this.resize(false);
      
      if (!this._context)
         this._context =
            this
            ._element
            .getContext("2d");
            
      return this._context;
         
   }
   
   hashFilter(key, value) {
      if ( [
            //"initialMatrix", 
            //"matrix",
            //"inverse",
           // "width",
          //  "height",
           //   "devicePixelRatio",
          //  "touchPoints",
             //  "_resized",
              "_context",
              "lastDrawTimestamp",
              "_element"
           ].includes(key) )
         return undefined;
         
      return super.hashFilter(
         key, value
      );
   }

   draw() {

      var canvas = this;
      
      window.requestAnimationFrame(
   
         function(timestamp) {
      
            if (timestamp <=
                canvas.lastDrawTimestamp
               )
            return;
         
            canvas.lastDrawTimestamp =
               timestamp;
       
            _draw();
         }
      );
      
      function _draw() {
         
         if (!canvas.resized)
            canvas.resize(false);

         var context =
            canvas.context;

         context.resetTransform();
   
         context.clearRect(
            0, 0,
            canvas._element.width,
            canvas._element.height
         );

         context.applyMatrix(
            canvas.matrix
         );

         context.save();
      
         var scale =
            context.getScale().x;

         context.lineWidth =
            0.5 / scale;
            
         context.setLineDash(
            [10 / scale,
             3 / scale,
             5 / scale,
             3 / scale]
         )
         
         var lines =
            Canvas
            .getCrossLines(canvas);
         
         lines.forEach(
            line => line.draw(context)
         );

         context.restore();
         
         canvas.layers.draw(context);
   
      }
   }
   
   get resized() {
      return this._resized;
   }
   
   resize(redraw = true) {
      
      var element = this._element;
      delete this._lines;
         
      var pixPerMm =
         window
         .getPixelsPerMillimeter();
         
      this.devicePixelRatio =
         window.devicePixelRatio;
      
      // set the canvas _elements
      // width in pixels
      element.width =
         window.innerWidth *
         this.devicePixelRatio;

      // set width in actual
      // millimeters
      this.width =
         element.width /
         pixPerMm.x /
         this.devicePixelRatio;
    
      // and the height
      
      // pixels
      element.height = 
         window.innerHeight *
         this.devicePixelRatio;

      // millimeters
      this.height =
         element.height /
         pixPerMm.y /
         this.devicePixelRatio

      this.initialMatrix =
      [
         pixPerMm.x, 0.0,
         0.0, -pixPerMm.y,
         this.width * pixPerMm.x / 2.0,
         this.height * pixPerMm.y / 2.0
      ]
      
      if (!this.matrix) {
         this.matrix =
            [...this.initialMatrix];
      }
      
      // reset the context
      this._context = null;
      
      this._resized = true;
      
      // reset topleft point
      this.topLeft = new Point(
         -this.width / 2,
         this.height / 2
      );
      
      // draw on the canvas
      if (redraw)
         this.draw()

   }
   
   get pixelSize() {
   
      var pixPerMm = window.
         getPixelsPerMillimeter();
         
      return new Point(
         1 /
            pixPerMm.x /
            this.devicePixelRatio /
            this.matrix[0],
         1 /
            pixPerMm.y /
            this.devicePixelRatio /
            this.matrix[3]
      );
   }
   
   hitTest(point) {
      var hit =
         this.layers.hitTest(point);
      return hit;
   }
   
   penDown(point) {
   
      // fix the points id
      var id = point.id;
      
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
      this._penMoved = false;
      this._startPoint = point.copy();
   }
   
   movePen(point) {
      // fix the points id
      var id = point.id;
      
      this._points.push(point);
   
      var context = this._context;
      
      context.lineTo(
         point.x,
         point.y
      );
   
      context.stroke();

   }
   
   penUp() {
      
      if (!this._points)
         return;
         
      var line =
         new Line(
            {
               points: this._points,
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         );

         
      // transform all the points in     
      // the line
      line.transformScreenToCanvas();

      // create the selected app
      // if there are none
      var selectedApp = this.selectedApp;
           
      if (!selectedApp)
      {

         // create the new app
         var newApp = new App(
            {
               canvas: this,
               layer:
                  this.layers.current,
               strokeStyle: "black",
               fillStyle: "yellow",
               lineWidth: 0.5
            }
         );
 
         // get its parent
         var parent =
            this.layers.current.editor;
            
         // add the new app to 
         // its parent
         parent.addChildApp(newApp);
            
         // make the new app
         // our selected
         this.selectedApp 
            = newApp;
         
         selectedApp = newApp;
         newApp.setStyle();

      }
         
      // add the line to the selected
      // app
      selectedApp.addLine(line);
      
      // make the line the selected app
      this
         .layers
         .current
         .selectedApp = selectedApp;
      
      // end the drawing line
      this._points = null;
      
      // save to session storage
      this.save();
   
      // redraw the scene
      this.draw();
   }
   
   click(point) {
   
      this.transformScreenToCanvas(
         point
      );
      
      var app = this.hitTest(point);
      
      var clicked = false;
      
      if (app)
         clicked = app.click(point);
      
      if (clicked)
         this.draw();
         
      return clicked;
   }
   
   longPress(point) {

      this.transformScreenToCanvas(
         point
      );
      
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
      
      var previouslySelected =
         this.layers
            .current
            .selectedApp;
      
 
        
      var app = this.hitTest(point);
      
      if (app)
         app.longPress(point);
      else if (previouslySelected) {
         this.layers
            .current
            .selectedApp = null;
      }
      else if (this.layers.length > 1)
      {

            var layer =
               this.layers.pop();
            
      }
     
      if (previouslySelected)
         previouslySelected.setStyle();
         
      this.save();
 
      this.draw();
      
   }
   
   remove() {
      document.body.removeChild(
         this._element
      );
   }
   
   get layer() {
      return this.layers.current;
   }
   
   get selectedApp() {
      return this.layer.selectedApp;
   }
   
   set selectedApp(value) {
      this.layer.selectedApp = value;
   }
   
   transform(from, to, scale) 
   {
      glMatrix.mat2d.translate(
         this.matrix,
         this.matrix,
         [ to.x, to.y ]
      );
  
      glMatrix.mat2d.scale(
         this.matrix,
         this.matrix,
         [ scale, scale ]
      );
   
      glMatrix.mat2d.translate(
         this.matrix,
         this.matrix,
         [ -from.x, -from.y ]
      );
   
      glMatrix.mat2d
         .invert(
            this.inverse,
            this.matrix
         );
      this.save();
      this.draw();
      
   }
   
   touchTransform() {
   
      // transform a copy of
      // points
      var canvas = this;
      
      var points =
         this
         .touchPoints.map(
            copyAndTransform
         );
     
      function copyAndTransform(point)
      {
         // copy the point
         var copy = point.copy();
         // transform the point
         canvas
            .transformScreenToCanvas(
               copy
            );
         return copy;
      }

      
      // points now has four points
      // Two points, from start fingers
      // and two points from moved
      // fingers
      var fromDistance = Point.distance(
         points[0],
         points[1]
      );
  
      var toDistance = Point.distance(
         points[2],
         points[3]
      );
   
      var scale =
         toDistance / fromDistance;
   
      var fromMiddle = Point.middle(
         points[0],
         points[1]
      );
 
      var toMiddle = Point.middle(
         points[2],
         points[3]
      );
   
      // perform the actual translate
      // and scale
      this.transform(
         fromMiddle,
         toMiddle,
         scale
      );
      

   }
   
   // transforms a point from screen
   // coordinates to canvas coordinates
   // this modifies the point on place
   transformScreenToCanvas(point) {
   
      var array = [point.x, point.y];
      
      glMatrix.vec2.transformMat2d(
         array,
         array,
         this.inverse
      );
      
      point.x = array[0];
      point.y = array[1];
      
      return point;
   }
   

   addLayer(editor) {
      editor.canvas = this;
      this.layers.push(editor);
   }

}

Canvas.ELEMENT_ID = "canvas";
Canvas.MAX_MOVE = 12; // Pixels
Canvas.LONG_PRESS_TIME = 300; // millis
Canvas.VIBRATE_TIME = 150; // millisecs
Canvas.ZOOM_INTENSITY = 0.5;

// create the cross
Canvas.getCrossLines =
function(canvas) {
   var lineWidth = 0.3;
   var lineColor = "grey";
   return [

      // top
      new Line(
      {
         points: [
            new Point( {
               x: 0,
               y: 0
            }),
            new Point(
            {
               x: 0,
               y: canvas.height / 2
            })
         ],
         lineColor,
         lineWidth
      } ),
      // bottom
      new Line(
      {
         points: [
            new Point( {
               x: 0,
               y: 0
            } ),
            new Point( {
               x: 0,
               y: -canvas.height / 2
            } )
         ],
         lineColor,
         lineWidth
      } ),
      // right
      new Line(
      {
         points: [
            new Point( {
               x: 0,
               y: 0
            } ),
            new Point( {
               x: canvas.width / 2,
               y: 0
            } )
         ],
         lineColor,
         lineWidth
      } ),
      // left
      new Line(
      {
         points: [
            new Point( {
               x: 0,
               y: 0
            } ),
            new Point( {
               x: -canvas.width / 2,
               y: 0
            } )
         ],
         lineColor,
         lineWidth
      } )
   ];
}

CanvasRenderingContext2D
   .prototype
   .getScale = 
function() {

   var pixPerMm =
      window.getPixelsPerMillimeter();
   
   
   var scale = {
      x: this.matrix[0] / pixPerMm.x,
      y: this.matrix[3] / pixPerMm.y
   }

   return scale;
}

CanvasRenderingContext2D
   .prototype
   .applyMatrix =
function(matrix) {
   this.matrix = matrix;
   this.setTransform(
      matrix[0],
      matrix[1],
      matrix[2],
      matrix[3],
      matrix[4],
      matrix[5]
   );
}
      
