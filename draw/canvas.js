class Canvas {
   #resized = false;
   #transformMatrix = null;
   #initialMatrix = null;
   #context = null;
   #element = null;
   #linePoints = null;
   #penMoved = false;
   #startPoint = null;
   #touchPoints = null;
   #topLeft = null;
   #lastDrawTimestamp = null;
   
   constructor(input) {
      var canvas = this;
 
      if (!input)
         input = {}

      Object.assign(this, input);
   
      var element =
         document.getElementById(Canvas.ELEMENT_ID);
      
      if (!element) {
         element =
            document
            .createElement("canvas");
         
         element.id = Canvas.ELEMENT_ID;
   
         setStyle(element);
      
         document.body.appendChild(
            element
         );
      
         this.#resized = false;
      }
   
      setEvents(element);
      this.#element = element;

      var longPressTimer = null;
      var longPressPoint = null;
      var context = null;
   
      window.addEventListener("resize",
         function() {
            canvas.#resized = false;
            canvas.resize(true);
         }
      );
   
      if (input.layers == null) {
         this.resize(false);
         this.layers = new Layers(
            {
               canvas: this
            }
         );
         var base = this.layers.push();
         base.layer = base;
      }

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
         
         if (event.touches)
            touchCount =
               event.touches.length;
         
         if (touchCount > 1) {
            // two or more fingers,
            // start scale/translate
            // mode
            canvas.#touchPoints =
               [
                  getPoint(event, 0),
                  getPoint(event, 1)
               ];
            
            // cancel any selected 
            // drawings
            canvas.#linePoints = null;
            
            // cancel long press timer
            clearLongPressTimer(
               "multiple"
            );
            
            return;
         }
         
            
         // cancel selected transform
         canvas.#touchPoints = null;

         // get the point and start
         // drawing
         var point = getPoint(
            event, 0
         );
   
         canvas.#startPoint =
            Point.copy(point);
         canvas.#penMoved = false;
         
         canvas.penDown(point);
        
         // Start the timer for long
         // touch
         longPressTimer =
            window.setTimeout(
               onlongpress,
               Canvas.LONG_PRESS_TIME
            );
         longPressPoint =
            Point.copy(canvas.#startPoint);

      }
      
      function onmove(event) {

         event.preventDefault();
         
         if (canvas.#touchPoints) {

             // get the next two points
             // for translate/scale
             canvas.#touchPoints.push(
                getPoint(event, 0)
             );
         
             canvas.#touchPoints.push(
                getPoint(event, 1)
             );

             // perform the transform
             canvas.touchTransform();
      
             // shift the two points
             // to replace the first ones
             canvas.#touchPoints
                 .shift();
             
             canvas.#touchPoints
                 .shift();
      
         }
         else if (canvas.#linePoints) {
      
            // get the next point
            // and move the pen to
            // there
            var point = getPoint(event, 0);
               
            canvas.movePen(point);
            
            // If we have moved greater
            // then 5 pix then
            // cancel the long press
            // timer
            if ( !canvas.#penMoved &&
                 Point.distance(
                    point,
                    canvas.#startPoint
                 ) > Canvas.MAX_MOVE )
            {
            
               clearLongPressTimer(
                  "move"
               );
               
               canvas.#penMoved = true;
            }
            
   
         }
      }
      
      function onend(event) {
         event.preventDefault();
      
         clearLongPressTimer("end");

         if (canvas.#touchPoints) {
            // stop transforming
            canvas.#touchPoints = null;
            canvas.endTouchTransform();
         }
         else if (canvas.#linePoints) {
            
            var clicked = false;
            if (!canvas.#penMoved) {
               clicked =
                  canvas.click(
                     canvas.#startPoint
                  );
            }
            
            if (!clicked) {
               canvas.penUp();
            }
         
            // stop drawing
            canvas.#linePoints = null;
         
            canvas.save();
         }
      
      }
      
      // mouse wheel used for zooming
      function onmousezoom(event) {
      
         event.preventDefault();
         
         var point = getPoint(event);
      
         canvas.transformScreenToCanvas(
            point
         );
    
         // Normalize wheel to +1 or -1
         var wheel = event.deltaY < 0 ?
            1 : -1;
         
         // Compute zoom factor
         var zoom = Math.exp(
            wheel * Canvas.ZOOM_INTENSITY
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
         canvas.#linePoints = null;
 
         canvas.longPress(longPressPoint);
         canvas.topLayer.save()
      }
      
      // Clear the timeout for long
      // press.
      function clearLongPressTimer(reason){
         
         if (longPressTimer) {
            window.clearTimeout(
               longPressTimer
            );
            longPressTimer = null;
         }
         
      }
      
      function getPoint(event, touchIndex) {

         var x, y;
   
         if (event.touches) {
            var touch =
               event.touches[
                  touchIndex
               ];
               
            x = touch.clientX,
            y = touch.clientY
         }
         else {
            x = event.clientX,
            y = event.clientY;
         }
   
         return new Point(
            {x, y}
         );
      }
      
   }
   
   get initialMatrix() {
      return this.#initialMatrix;
   }
   
   get transformMatrix() {
   
      if (this.#transformMatrix)
         return this.#transformMatrix;
         
      this.#transformMatrix =
         glMatrix.mat2d.create();
  
      return this.#transformMatrix;
      
   }
   
   set transformMatrix(value) {
      this.#transformMatrix = value;
      
            
      if (!this.matrix) {
         this.matrix =
            glMatrix.mat2d.create();
         
      }
      
      glMatrix.mat2d.multiply(
         this.matrix,
         this.initialMatrix,
         this.#transformMatrix
      );
      
     // console.log("canvas.js 334:" + Array.from(this.matrix));
      
   }
   
   get context() {
      if (!this.#resized)
         this.resize(false);
      
      if (!this.#context)
         this.#context =
            this.#element.getContext("2d");
            
      return this.#context;
         
   }
   
   drawCrossLines(canvas, context) {
      context.applyMatrix(
         canvas.matrix
      );
         
      var scale =
         context.getScale().x;
            
      context.setLineDash(
         [10 / scale,
          2 / scale,
          3 / scale,
          2 / scale]
      );


      var lines =
         getCrossLines(canvas);
         
      lines.forEach(
         function(line) {
            line.draw(context);
         }
      );
   
   
      function getCrossLines(canvas) {
         var lineWidth = 0.3;
         var strokeStyle = "black";
         var layer = canvas.topLayer;
         return [

            // top
            new Line(
            {
               canvas: canvas,
               layer: layer,
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
               strokeStyle: strokeStyle,
               lineWidth: lineWidth
            } ),
            // bottom
            new Line(
            {
               canvas: canvas,
               layer: layer,
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
               strokeStyle: strokeStyle,
               lineWidth: lineWidth
            } ),
            // right
            new Line(
            {
               canvas: canvas,
               layer: layer,
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
               strokeStyle: strokeStyle,
               lineWidth: lineWidth
            } ),
            // left
            new Line(
            {
               canvas: canvas,
               layer: layer,
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
               strokeStyle: strokeStyle,
               lineWidth: lineWidth
            } )
         ];
      }
   }
   
   draw(forceDraw = false) {
      
      var element = this.#element;
      var resized = this.#resized;
      var canvas = this;
      
      if (forceDraw) {
         _draw();
         return;
      }
      
      window.requestAnimationFrame(
   
         function(timestamp) {
      
            if ( timestamp <=
                 canvas.#lastDrawTimestamp
               )
            return;
         
            canvas.#lastDrawTimestamp =
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

         canvas.layers.draw(context);
   
      }
      

   }
   
   resize(redraw = true) {
         
      var pixPerMm =
         window
         .getPixelsPerMillimeter();
         
      this.devicePixelRatio =
         window.devicePixelRatio;
      
      var element = this.#element;
      
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

      this.#initialMatrix =
         glMatrix.mat2d.fromValues(
            pixPerMm.x,
            0.0,
            0.0,
           -pixPerMm.y,
            this.width  * pixPerMm.x / 2.0,
            this.height * pixPerMm.y / 2.0
         );
      
      
      if (!this.#transformMatrix)
         this.transformMatrix =
            glMatrix.mat2d.create();
      
      // reset the context
      this.#context = null;
      
      this.#resized = true;
      
      // reset topleft point
      this.#topLeft = new Point(
         {
            x: -this.width / 2,
            y: this.height / 2
         }
      );
      
      // draw on the canvas
      if (redraw)
         this.draw()

   }
   
   getPixelSize() {
   
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
   
   hitTest(point, event) {
      var hit =
         this.topLayer.hitTest(point, event);
      return hit;
   }
   
   penDown(point) {
   
      // Fix the points id in time and
      // space
      point["="];
      
      var context = this.context;
      
      context.resetTransform();
      context.lineWidth = 0.3;
      
      context.strokeStyle = "black";
   
      context.beginPath();
   
      context.moveTo(
         point.x,
         point.y
      );
      
      this.#linePoints = [point];
      this.#penMoved = false;
      this.#startPoint = Point.copy(point);
   }
   
   movePen(point) {
      // fix the points id
      point["="];
      
      this.#linePoints.push(point);
   
      var context = this.context;
      
      context.lineTo(
         point.x,
         point.y
      );
   
      context.stroke();
   }
   
   penUp() {
      
      if (!this.#linePoints) {
         return;
      }
      
      var position = "end";
      
      var line = createLine(this.#linePoints);
   
      var parent = getParent(line);

      parent.addChild(line, position);
      
      // find all contained children
      var children = [];
      findChildren(this.topLayer);

      // transfer the contained children
      // from their current parent, to
      // the new line.
      children.forEach(
         function(child) {
            child.parent.removeChild(child);
            line.addChild(child);
         }
      );
      
      // end the drawing line
      this.#linePoints = null;

      this.topLayer.save();
      
      // redraw the scene
      this.draw();
      
      function createLine(points) {
         // transform all the points in     
         // the line from screen coordinates
         // to canvas coordinates
         points.forEach(
            function(point) {
               canvas
                  .transformScreenToCanvas(
                     point
                  );
            }
         );
         
         // see if the line connects
         // to objects
         var fromPoint = points[0];
         var toPoint   = points[
            canvas.#linePoints.length - 1
         ];
      
         var fromObject = canvas.hitTest(fromPoint);
         var toObject = canvas.hitTest(toPoint);
      
         // Determine if we want a connector
         var isConnector = false;
         if ((fromObject && toObject) &&
            (fromObject != toObject)) {
            var fromLabel =
               fromObject.label ?
                  fromObject.label :
                  fromObject.name;
                               
            var toLabel =
               toObject.label ?
                  toObject.label :
                  toObject.name;
                  
            if (confirm(
                "Connect " + fromLabel + 
                " to " + toLabel))
               isConnector = true;
         }
      
         // create the line
         var line;         
         if (isConnector) {
            line = new Connector(
               {
                  from: fromObject,
                  to: toObject,
                  points: points,
                  lineWidth: 0.5,
                  strokeStyle: "black",
                  layer: canvas.topLayer
               }
            );
            // Connect from -> outputs
            fromObject.addOutConnector(line);
            toObject.addInConnector(line);
            position = "start";
         }
         else {
            line = new Line(
               {
                  points: points,
                  lineWidth: 0.5,
                  strokeStyle: "black",
                  layer: canvas.topLayer
               }
            );
            position = null;
         }
         return line;
      }
      
      function getParent(line) {
         // get the deepest child
         // that contains the line
         var parent =
            canvas.topLayer.contains(
               line.dimensions
            );
         
         if (!parent)
            parent = canvas.topLayer;
            
         return parent;
      }
      
      function findChildren(parent) {
      
         if (parent.dimensions.contains(
                line.dimensions)) {
            // this parent intersects our
            // line, so search each child
            // to see if any are contained
            parent.children.forEach(
               function(child) {
                  if (line
                      .dimensions.contains(
                         child.dimensions) &&
                      child != line)
                     // Found a contained child
                     children.push(child);
                  else if (child != line)
                     // continue to search tbis child
                     findChildren(child);
                         
               }
            );
         }
         
      }
     
   }
   
   click(point) {
      console.log("Canvas.click");
      this.transformScreenToCanvas(
         point
      );
      
      var drawing = this.hitTest(point, "click");
      
      if (drawing) {
         
         drawing.click(point);
      }
      
      this.draw();
         
      return true;
   }
   
   longPress(point) {

      this.transformScreenToCanvas(
         point
      );
      
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );
         
      var hit = this.hitTest(point, "longpress");

      if (hit) {
         hit.longPress(point);
      }
      else {
         if (this.layers.length > 1)
         {
            this.layers.pop();
         }

      }
      
      this.draw();
      
 
   }
   
   remove() {
      document.body.removeChild(
         this.#element
      );
   }
   
   get topLeft() {
      return this.#topLeft;
   }
   
   get topLayer() {
      return this.layers.top;
   }

   get selectionLayer() {
      //return this.topLayer;
      if (this.layers.length == 1)
         return null;
      var layer = this.layers.stack[
         this.layers.length - 2
      ];
      
      return layer;
   }
   
   get selection() {
      if (this.selectionLayer == null)
         return null;
      return this.selectionLayer.selection;
   }
   
   set selection(value) {

      //document.writeln("<pre>" + this.topLayer + "</pre>");
      this.selectionLayer.selection = value;
   }
   
   transform(from, to, scale) {
      var layer = this.topLayer;
      
      glMatrix.mat2d.translate(
         layer.transformMatrix,
         layer.transformMatrix,
         [ to.x, to.y ]
      );
  
      glMatrix.mat2d.scale(
         layer.transformMatrix,
         layer.transformMatrix,
         [ scale, scale ]
      );
   
      glMatrix.mat2d.translate(
         layer.transformMatrix,
         layer.transformMatrix,
         [ -from.x, -from.y ]
      );
           
      glMatrix.mat2d.invert(
         layer.inverseTransformMatrix,
         layer.transformMatrix
      );
      
      glMatrix.mat2d.multiply(
         layer.matrix,
         this.initialMatrix,
         layer.transformMatrix
      );
      
      glMatrix.mat2d
         .invert(
            layer.inverse,
            layer.matrix
         );
         
      this.draw();
      
   }
   
   touchTransform() {
   
      // transform a copy of
      // points
      var canvas = this;
      
      var transformPoints =
         this.#touchPoints.map(
            copyAndTransform
         );
     
      function copyAndTransform(point)
      {
         // copy the point
         var copy = Point.copy(point);
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
         transformPoints[0],
         transformPoints[1]
      );
  
      var toDistance = Point.distance(
         transformPoints[2],
         transformPoints[3]
      );
   
      var scale =
         toDistance / fromDistance;
   
      var fromMiddle = Point.middle(
         transformPoints[0],
         transformPoints[1]
      );
 
      var toMiddle = Point.middle(
         transformPoints[2],
         transformPoints[3]
      );
   
      // perform the actual translate
      // and scale
      this.transform(
         fromMiddle,
         toMiddle,
         scale
      );
      

   }
   
   endTouchTransform() {
      this.topLayer.save();
   }
   
   // transforms a point from screen
   // coordinates to canvas coordinates
   // this modifies the point on place
   transformScreenToCanvas(point) {
   
      var array = [point.x, point.y];
      
      glMatrix.vec2.transformMat2d(
         array,
         array,
         this.topLayer.inverse
      );
      
      point.x = array[0];
      point.y = array[1];
      
      return point;
   }
   
}

Canvas.load = function() {

   var storage = Memory.storage;
   
   var key = storage.getItem("Canvas");

   var canvas = null;
   
   if (key) {
      console.log("Load canvas from " + key);
      canvas = Memory.fetch(key);
   }
   else {
      console.log("New canvas");
      canvas = new Canvas();
      key = canvas.save();
      storage.setItem("Canvas", key);
   }
   
   return canvas;
}
/*
Canvas.save = function(object) {

   console.log("Saving");
   
   if (!canvas)
      return;
   
   // Save the child object
   object.save();
   
   // Reload the canvas
   canvas = Memory.fetch(
      canvas["="].key
   );
 
}
*/
Canvas.ELEMENT_ID = "canvas";
Canvas.MAX_MOVE = 18; // Pixels
Canvas.LONG_PRESS_TIME = 300; // millis
Canvas.VIBRATE_TIME = 150; // millisecs
Canvas.ZOOM_INTENSITY = 0.5;


CanvasRenderingContext2D
   .prototype
   .getScale = 
function() {


   var pixPerMm =
      window.getPixelsPerMillimeter();
   
  
   var scaleVector = {
      x: this.matrix[0] / pixPerMm.x,
      y: -this.matrix[3] / pixPerMm.y
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
      matrix[0],
      matrix[1],
      matrix[2],
      matrix[3],
      matrix[4],
      matrix[5]
   );
}
      
