class Canvas extends UserInput {
   _resized = false;
   transformMatrix = new Matrix();
   _initialMatrix = new Matrix();
   _context = null;
   _element = null;
   _topLeft = null;
   _lastDrawTimestamp = null;
   _points = [];
   _loaded = false;
   static ELEMENT_ID = "canvas";
   static MAX_MOVE = 18; // Pixels
   static LONG_PRESS_TIME = 300; // millis
   static VIBRATE_TIME = 150; // millisecs
   static ZOOM_INTENSITY = 0.5;
   constructor(input) {
      super(getElement());

      canvas = this;
 
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
      
      this._resized = false;
      
      setStyle(element);
      
      this._element = element;
      
      var context = null;
   
      window.addEventListener("resize",
         function() {
            canvas._resized = false;
            canvas.resize(true);
         }
      );
      
      this.resize(false);

      if (input.layers == null) {

         this.layers = new Layers(
            {
               canvas: this
            }
         );
         // Starting layer,
         // Create a blank drawing
         var baseLayer = new Drawing(
            {
               canvas: this
            }
         );
         this.layers.push(baseLayer);
      }
      else {
         // Layers copy constructor
         var layers = input.layers;
         layers.canvas = this;
         this.layers = new Layers(
            layers
         );
      }
      
      this.save().then(
         (keys) => {
            storage.setItem(
               "Canvas.key", keys[0]
            );
            canvas._loaded = true;
         }
      );

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
      return this._initialMatrix;
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
      
            if ( !canvas._loaded )
               return;
               
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

         
         canvas.layers.fetch().then(
            layers => layers.draw()
         );
   
      }
      

   }
   
   resize(redraw = true) {
         
      var pixPerMm =
         window
         .getPixelsPerMillimeter();
         
      this.devicePixelRatio =
         window.devicePixelRatio;
      
      var element = this._element;
      
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
         this.devicePixelRatio;
      
      // Coordinate system millimetrs
      // Origin: bottom left
      // Bottom Up
      this._initialMatrix = new Matrix();
      this._initialMatrix.scaleSelf(
         pixPerMm.x,
         -pixPerMm.y,
         1.0
      );
      
      this._initialMatrix.translateSelf(
         0,
         -this.height,
         0
      );
      
      if (!this.transformMatrix)
         this.transformMatrix =
            new Matrix();
      
      // reset the context
      this._context = null;
      
      this._resized = true;
      
      // reset topleft point
      this._topLeft = new Point(
         {
            x: 0,
            y: this.height
         }
      );
      
      this.scrollToTop();
      
      // draw on the canvas
      if (redraw)
         this.draw()

   }
   
   scrollToTop() {
      document.body.scrollTop = 0; // For Safari
      document.documentElement.scrollTop = 0; // For Chrome, Firefox, IE and Opera
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
   
   async hitTest(point, event) {
      var top = await this.topLayer();
      console.log(0);
      var hit =
         await top.hitTest(point, event);
      console.log(1);
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
      
      this._points = [point];

   }
   
   movePen(point) {
      // fix the points id
      point["="];
      
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
      
      var position = "end";
 
      var line = await createLine(this._points);
 
      var parent = await getParent(line);

      parent.addChild(line, position);

      var top = await this.topLayer();
      
      // find all contained children
      var children = [];
      findChildren(top);

      // transfer the contained children
      // from their current parent, to
      // the new line.
      children.forEach(
         (child) => {
            child.parent.removeChild(child);
            line.addChild(child);
         }
      );
      
      // end the drawing line
      this._points = null;
      
      selection = line;

      var keys = await this.save();
      
      // redraw the scene
      this.draw();
      
      async function createLine(points) {
         
         points.forEach(
            async function(point) {
               await canvas
                  .transformScreenToCanvas(
                     point
                  );
            }
         );
        
         // see if the line connects
         // to objects
         var fromPoint = points[0];
         var toPoint =
            points[
               points.length - 1
            ];
   
         var fromObjectPromise =
            canvas.hitTest(fromPoint);
         var toObjectPromise =
            canvas.hitTest(toPoint);
       
         var result = await Promise.all(
            fromObjectPromise, toObjectPromise
         );
         
         var fromObject = result[0];
         var toObject = result[1];
         
         
         
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
      
         var top = await canvas.topLayer();
         
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
                  layer: top
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
                  layer: top
               }
            );
            console.log(line);
            position = "end";
         }
         return line;
      }
      
      async function getParent(line) {
         var top = await canvas.topLayer();
         
         console.log(line.dimensions);
         
         // get the deepest child
         // that contains the line
         var parent =
            top.contains(
               line.dimensions
            );
         
         if (!parent)
            parent = top;
            
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
      
      this.save();
      
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
      
         this.selection = null;
         
         if (this.layers.length > 1)
            this.layers.pop();

      }
      
      this.save();
      
      this.draw();
      
   }
   
   remove() {
      document.body.removeChild(
         this._element
      );
   }
   
   get topLeft() {
      return this._topLeft;
   }
   
   async topLayer() {
      
      var layers = await this.layers.fetch();
      var top = await layers.top();

      return top;
   }

   get selectionLayer() {
      
      var layer;
      if (this.layers.length > 1)
         layer = this.layers.stack[
            this.layers.length - 2
         ];
      else
         layer = this.layers.stack[0];
         
      return layer;
      
   }
   
   get selection() {
      return this.selectionLayer.selection;
   }
  
   set selection(value) {
      this.selectionLayer.selection = value;
   }
   
   transform(from, to, scale) {
      var layer = this.topLayer()
      layer.transformMatrix.translateSelf(
         to.x, to.y, 0
      );
      
      layer.transformMatrix.scaleSelf(
         scale,
         scale,
         1
      );
      
      layer.transformMatrix.translateSelf(
         -from.x, -from.y, 0
      );
      
      layer.inverseTransformMatrix =
         layer.transformMatrix.inverse();
         
      layer.matrix =
         this.initialMatrix.multiply(
            layer.transformMatrix
         );
      
      layer.inverse =
         layer.matrix.inverse();
      
      this.draw();
      
   }
   
   
   endTouchTransform() {
      this.save();
   }
   
   // transforms a point from screen
   // coordinates to canvas coordinates
   // this modifies the point in place
   async transformScreenToCanvas(point) {
   
      var p = new DOMPoint(
         point.x,
         point.y,
         0
      );
      
      var inverse = await
         this.topLayer().inverse.fetch();
      
      p = inverse.transformPoint(
             p
          );
      
      point.x = p.x;
      point.y = p.y;
      
      return point;
   }
   
   static load()
   {
      var storage = Memory.storage;
      return storage
         .getItem(
            "Canvas.key"
         ).then(
            (key) => {
               var promise;
               if (key)
                  promise = Memory.fetch(key);
               else {
                  canvas =
                     new Canvas();
                  promise =
                     Promise.resolve(
                        canvas
                     );
               }
               return promise;
            }
         )
         .catch(
            (error) => {
               alert("Canvas.load: " + error.stack);
            }
         );

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
