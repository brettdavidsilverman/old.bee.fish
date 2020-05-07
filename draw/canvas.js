class Canvas extends UserInput {
   #resized = false;
   #transformMatrix = null;
   #initialMatrix = null;
   #context = null;
   #element = null;
   #topLeft = null;
   #lastDrawTimestamp = null;
   #points = [];
   
   constructor(input) {
      super(getElement());
      
      var canvas = this;
 
      if (!input)
         input = {}

      Object.assign(this, input);
   
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
      
      this.#resized = false;
      
      setStyle(element);
      
      this.#element = element;
      
      var context = null;
   
      window.addEventListener("resize",
         function() {
            canvas.#resized = false;
            canvas.resize(true);
         }
      );
      
      this.resize(false);
      
      if (!input.layers) {
         
         this.layers = new Layers(
            {
               canvas: this
            }
         );
         var base = this.layers.push();
         base.layer = base;
      }
      else {
         var input = input.layers;
         input.canvas = this;
         this.layers = new Layers(
            input
         );
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
         this.#initialMatrix,
         this.#transformMatrix
      );
      

   }
   
   get context() {
      if (!this.#resized)
         this.resize(false);
      
      if (!this.#context)
         this.#context =
            this.#element.getContext("2d");
            
      return this.#context;
         
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
      
      this.#points = [point];

   }
   
   movePen(point) {
      // fix the points id
      point["="];
      
      this.#points.push(point);
   
      var context = this.context;
      
      context.lineTo(
         point.x,
         point.y
      );
   
      context.stroke();
   }
   
   penUp() {
      
      if (!this.#points) {
         return;
      }
      
      var canvas = this;
      
      var position = "end";
      
      var line = createLine(this.#points);
      
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
      this.#points = null;
      
      Canvas.save();
      
      // redraw the scene
      this.draw();
      
      function createLine(points) {

            
         // see if the line connects
         // to objects
         var fromPoint = Point.copy(
            points[0]
         );
         canvas.transformScreenToCanvas(
           fromPoint
         );
         
         var toPoint = Point.copy(
            points[
               points.length - 1
            ]
         );
         canvas.transformScreenToCanvas(
            toPoint
         );
         
         var fromObject = canvas.hitTest(fromPoint);
         var toObject = canvas.hitTest(toPoint);
      
    
         // Determine if we want a connector
         var isConnector = false;
         if ((fromObject && toObject) &&
            (fromObject != toObject)) {
            var fromLabel =
               fromObject.label ?
                  fromObject.label :
                  fromObject["="].name;
                               
            var toLabel =
               toObject.label ?
                  toObject.label :
                  toObject["="].name;
                  
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
      
      this.transformScreenToCanvas(
         point
      );
      
      var drawing = this.hitTest(point, "click");
      
      if (drawing) {
         
         drawing.click(point);
      }
      
      Canvas.save();
      
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
      
      Canvas.save();
      
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
         return this.layers.stack[0];
         
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
   
   
   endTouchTransform() {
      Canvas.save();
   }
   
   // transforms a point from screen
   // coordinates to canvas coordinates
   // this modifies the point in place
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
      canvas = Memory.fetch(key);
   }
   else {

      canvas = new Canvas();
      key = canvas.save();
      storage.setItem("Canvas", key);
   }
   
   canvas.resize(true);
   
   return canvas;
}

Canvas.save = function() {
   
   // Save the child object
   var key = canvas.save();
   
   Memory.storage.setItem("Canvas", key);
 
}

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
      
