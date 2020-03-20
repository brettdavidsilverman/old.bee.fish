var selectedDrawing = null;

function Canvas(input) {

   var canvas = this;
   
   if (input && input.id)
      this.id = input.id;
   
   var parent = input.parent;
   if (!parent)
      parent = document.body;
   
   this.drawings = input.drawings;
   if (!this.drawings)
      this.drawings = [];
      
   this.fitToParent = input.fitToParent;
   if (!this.fitToParent)
      this.fitToParent = false;
   
   var pixelRatio = 
      window.devicePixelRatio;
   
   this.pixelsPerMillimeter =
      input.pixelsPerMillimeter;
      
   if (!this.pixelsPerMillimeter)
      this.pixelsPerMillimeter =
         Canvas.getPixelsPerMillimeter();
   
   var pixPerMm = this.pixelsPerMillimeter;
   
   var element = input.element;
   
   if (!element) {
      element = document
         .createElement("canvas");
      element.id = this.id;
      parent
         .appendChild(element);
   }
   
   // Matricies
   
   this.matrix = Matrix2d.identity;
   this.mmToPixMatrix = Matrix2d.identity;
   this.transformMatrix = Matrix2d.identity;
   
   
   this.width = input.width;
   if (!this.width)
      this.fitToParent = true;;
   
   this.height = input.height;
   if (!this.height)
      this.fitToParent = true;
   
   var uiEvents;
   
   if (input.UIEvents)
      uiEvents = new input.UIEvents(
         {
            element: element,
            pixelsPerMillimeter: pixPerMm
         }
      );
   else
      uiEvents = {}
  
   window.onhashchange =
      function(event) {
         canvas.load();
      }
    
   window.onresize =
      function(event) {

         canvas.resize(
            canvas.width,
            canvas.height
         );
         
      }
        
   var currentLine = null;
   var context = null;
   
   // declare canvas on uiEvents
   declareCanvasEvents();

   function declareCanvasEvents() {
   
      canvas.onclick =
         function(point) {}
      
      canvas.oncontextmenu =
         function(point) {}
         
      canvas.onpendown =
         function(point) {}
         
      canvas.onmovepen =
         function(point) {}
         
      canvas.onpenup =
         function(point) {}
         
      canvas.onsetmatrix = 
         function() {}
   
      canvas.ondraw =
         function(context) {}
   
      canvas.onresize =
         function(width, height) {}
   
      canvas.ontransform =
         function(matrix) {}
      
   }

   this.getContext = function() {
      return context;
   }
   
   this.getElement = function() {
      return element
   }
   
   // click on the drawing
   uiEvents.onclick = function(point) 
   {
      var hit = canvas.hitTest(point);
      
      point = transformPoint(point);
      
      if (hit && selectedDrawing == hit)
         hit.click(point);
      else
         selectedDrawing = hit;
      
      canvas.draw();
    
      canvas.onclick(point);
    
   }
   
   uiEvents.oncontextmenu =
      function(point)
   {
      
      var hit = canvas.hitTest(point);
      
      point = transformPoint(point);
      
      selectedDrawing = hit;
      
      canvas.draw();
            
      if (selectedDrawing)
         selectedDrawing
         .contextMenu(point);
      
      canvas.oncontextmenu(point);
      
   }
   
   this.hitTest = function(point) {
    
      var root = new Node(
         {
            leaf: this,
            getChildren: 
               function(leaf) {
                  if (leaf == canvas)
                     return canvas.drawings
                  else // drawing
                     return leaf.children;
               }
            
         }
      );
      
      var hits = [];

      for (node = root;
           node;
           node = node.next()) {
   
         if (node == root)
            continue;
            
         var drawing = node.leaf;
 
         if (drawing.hitTest(
                point,
                canvas.transformMatrix
             )
            )

            hits.push(drawing);
         else
            // Missed this node, so
            // skip its children
            delete node.children;
        
      }
    
      hits = hits.sort(
         function compare(a, b) {
            return b.level -
                   a.level;
         }
      );
      
      if (hits.length)
         return hits[0];
      else
         return null;
     
   }   

   uiEvents.onpendown = function(point) {
   
      currentLine = new Line(
         {
            start: point,
            points: []
         }
      );
      
      context.resetTransform();
      canvas.mmToPixMatrix.applyToContext(
         context
      );
      context.strokeStyle = "blue";
      context.lineWidth = 1/canvas.scale;
      context.beginPath();
      context.moveTo(point.x, point.y);
      canvas.onpendown(point);
   }

   uiEvents.onmovepen = function(point) {

      context.lineTo(point.x, point.y);
      
      context.stroke();
      
      currentLine.points.push(point);

      canvas.onmovepen(point);
      
   }

   uiEvents.onpenup = function(point) {
      currentLine = transformLine(
         currentLine
      );
         
      debug.add("Canvas.onpenup.line.points", currentLine.points.length);
      
      if (selectedDrawing) {
         if (!selectedDrawing
              .dimensions
              .isPointInside(
                currentLine.start
            )) {
            selectedDrawing = null;
         }
      }
       
      if (!selectedDrawing) {
         selectedDrawing =
         new Drawing(
            {
               parent: this,
               code: "this.label = prompt(\"Label?\")"
            }
         );
         canvas.drawings.push(selectedDrawing);
         debug.add("Canvas.drawings.push", canvas.drawings.length);
      }

      selectedDrawing.addLine(
         currentLine
      );
         
      currentLine = null;
      
      canvas.draw();
      
      canvas.onpenup();
      
      function transformLine(line) {
         
         transformPoint(
            line.start
         );
         
         line.points.forEach(
            function(point) {
               transformPoint(point);
            }
         );
         
         line
            .calculateDimensions();
            
         return line;
         
      }
   }
   
   uiEvents.oncancelpen = function()
   {
      currentLine = null;
      
      canvas.draw();
   }
   
   function transformPoint(point) {
   
      debug.add("canvas.transformPoint", "fuck, should this be the same for hitTest and transformLine?");
       
      var transformedPoint =
         canvas.transformMatrix
        // .inverse
         .transformPoint(point);
         
      point.x = transformedPoint.x;
      point.y = transformedPoint.y;

      //debug.add("canvas.transformPoint", Point.toString(point));
      
      return point;
   }


   uiEvents.ontransform = function(
         scale,
         time1Points,
         time2Points) 
   {
      // https://www.erikpaluka.com/blog/rst/
      // M' = T'S(-T)M
      
      time1Points[0] = transformPoint(Point.clone(time1Points[0]));
      time1Points[1] = transformPoint(Point.clone(time1Points[1]));
      time2Points[0] = transformPoint(Point.clone(time2Points[0]));
      time2Points[1] = transformPoint(Point.clone(time2Points[1]));
      
      // get the variables we need for the transform.
         
      var M = canvas.transformMatrix;
         
      var T = Point.middle(
         time1Points[0],
         time1Points[1]
      );

      var S = scale;

      var Tprime = Point.middle(
         time2Points[0],
         time2Points[1]
      );
      
      // M' = M.(-T).S.'T
      // Translate back to the origin,
      // scale, then translate to the
      // new position
      var Mprime =
         M
         .translate(Point.negative(T))
         .scale(S)
         .translate(Tprime);

      // Inverse' = 
      //  I.(-T').(1/S).T.Inverse
      //  Inverse is the reflection
      // (or opposite) of M
      
      var inverse = M.inverse;
         
      var inversePrime =
         Matrix2d.identity
         .translate(
            Point.negative(Tprime)
          )
         .scale(Point.reciprocal(S))
         .translate(T)
         .multiply(inverse);
         
      Mprime.inverse = inversePrime;
      
      canvas.setTransform(
         Mprime
      );
      
   }
         
   this.setTransform = function(m) {
      
      this.transformMatrix = m;
      
      this.matrix =
         this.mmToPixMatrix
         .multiply(this.transformMatrix);
      
      this.matrix.inverse =
         this.transformMatrix.inverse
         .multiply(this.mmToPixMatrix.inverse);
         
      this.lineWidth =
         1 / this.matrix.getScale().x;
         
      this.draw();
      
      this.ontransform(
         this.matrix
      );
      
   }
   
 
   // Enque the draw() function,
   // when the window requests the
   // next animation frame.
   // make sure we only queue the
   // function once, regardless of
   // how many times we are called
   // unless the now parameter is set,
   // then it draws straight away
   var lastTimestamp = null;
   
   this.draw = function (now) {
      
      if (now)
         draw(lastTimestamp);
      else {
         window.requestAnimationFrame(
            function (timestamp) {
               if (lastTimestamp !=
                   timestamp)
                  draw(timestamp);
         
               lastTimestamp =
                  timestamp;
            }
      
         );
         
         // while waiting
         // for animation frame,
         // calculate layout
         calculateDimensions();

      }
      
   
      function draw(timestamp) {
         time = timestamp;

         debug.add("draw length", canvas.drawings.length)
         var context =
            canvas.getContext();
         context.matrix = canvas.matrix;
         context.resetTransform();

         context.clearRect(0, 0,
            element.width,
            element.height
         );
         
         context.strokeStyle = "blue";
         
         context.lineWidth = canvas.lineWidth;
         
         
         var root = new Node(
            {
               leaf: canvas,
               getChildren:
                  function getChildren(leaf) {
                     if (leaf == canvas)
                        return canvas.drawings;
                     else
                        // leaf is a drawing
                        return leaf.children;
                  }
            }
         );
         
         for ( var node = root;
               node != null;
               node = node.next()
             )
         {
          
            if (node.leaf == canvas)
               continue;
               
            var drawing = node.leaf;
   
            if (
                !Bool(drawing.visible)) {
               delete node.children;
               continue;
            }
            
            context.save();
               
               drawing.draw(
                  context
               );
            
            context.restore();
            
         }
      
         canvas.ondraw(context);
      
      }
      
      function calculateDimensions() {
      
         canvas.drawings.forEach(
            function(drawing) {
               drawing
               .calculateDimensions();
            }
         );
         
      }
      

   }
   
   this.resize = function(width, height) {
      
      pixelRatio = window.devicePixelRatio;
      
      var pixPerMm = this.pixelsPerMillimeter;
      
      if (this.fitToParent) {
         fitToParent();
      }
      
      setDimensions();
      setOrientation();
      setMatrix();
      
      context = element.getContext("2d");
      canvas.lineWidth = 1/canvas.matrix.getScale().x;
 
      canvas.draw();
      
      debug.add("Canvas.resize (mm)", this.width + ", " + this.height);
      debug.add("Canvas.resize (px)", this.pixelWidth + ", " + this.pixelHeight);

      canvas.onresize(
         this.width,
         this.height
      );
      
      function fitToParent() {
        
         var rect =
            parent
            .getBoundingClientRect();
            
         width =
            rect.width / 
            pixPerMm.x *
            pixelRatio;
            
         height =
            rect.height /
             pixPerMm.y *
             pixelRatio;

      }
      
      function setMatrix() {
         
         canvas.mmToPixMatrix =
            Matrix2d.identity
            .scale(
               {
                  x: pixPerMm.x,
                  y: -pixPerMm.y 
               }
            )
            .translate(
               {
                  x: 0,
                  y: canvas.pixelHeight
               }
            );
         
         
         canvas.mmToPixMatrix.inverse =
            Matrix2d.identity
            .translate(
               {
                  x: 0,
                  y: -canvas.pixelHeight
               }
            )
            .scale(
               {
                  x: 1 / pixPerMm.x,
                  y: 1 / -pixPerMm.y 
               }
            );
         
         canvas.transformMatrix =
            Matrix2d.identity;
            
         canvas.transformMatrix.inverse =
            Matrix2d.identity;
            
         canvas.matrix =
            canvas.mmToPixMatrix
            .multiply(canvas.transformMatrix);
            
         canvas.matrix.inverse =
            canvas.transformMatrix.inverse
            .multiply(canvas.mmToPixMatrix.inverse);
            
         debug.add("canvas.matrix", canvas.matrix);
         debug.add("canvas.matrix.inverse", canvas.matrix.inverse);

      }
      
      function setDimensions() {
      
         var pixPerMm =
            canvas.pixelsPerMillimeter;
            
         canvas.width =
                width;
            
         canvas.height =
                height;
         
         canvas.pixelWidth =
                     width *
                pixPerMm.x;
         
         canvas.pixelHeight =
                     height *
                 pixPerMm.y;
         
         element.width =
            canvas.pixelWidth;
            
         element.height = 
            canvas.pixelHeight;
         
         element.style.width =
            canvas.width + "mm";
         
         element.style.height =
            canvas.height + "mm";
 
      }

      
      
      function setOrientation() {
         if (height >= width)
             
            canvas.orientation =
               "portrait";
         else
            canvas.orientation =
               "landscape";
      }

   }
   
   
   this.toString = function () {
      return JSON.stringify(this);
   }
   

   this.load = function() {

      var hash =
          document
          .location
          .hash.substr(1);
          
      if (!hash.length)
         return;
         
      var object = decode(hash);
      
      this.drawing = new Drawing(
         object
      );
      
      this.draw();
      debug.add("canvas.load.hash.drawing.children", this.drawing.children.length);
   }
   
   this.save = function() {
   
      var hash =
         encode(this.drawing);
      // set the documents hash
      document.location.hash =
         "#" + hash;
      // this should trigger a hash
      // change that will intern load
      // the drawing again
   }
   
   this.drawCross = function(context) {
      this.mmToPixMatrix.applyToContext(context);
      context.strokeStyle = "red";
      context.lineWidth = this.lineWidth;
      context.beginPath();
         
      context.moveTo(0, 0);
      
      context.lineTo(
         this.width,
         this.height
      );
      
      context.moveTo(
         this.width,
         0
      );
      
      context.lineTo(
         0,
         this.height
      );
      
      context.stroke();
   }
   
   this.loadButton = function(id, parent) {
   
      // load the button
      var button = buttons[id];
      button.id = id;
      button.parent = parent;
      
      var drawing = 
         new Drawing(button);

      return drawing;
   }
   
   this.removeDrawing = function(drawing) {
     
      var index = 
         this.drawings
         .indexOf(drawing);
         
      if (index >= 0)
         this.drawings
         .splice(index, 1);
   }
 
   this.resize(
      this.width,
      this.height
   );
   
   return this;
}


Canvas.getPixelsPerMillimeter = function() {

   var div = document.createElement("div");
   
   div.style.width = "10mm";
   div.style.height = "10mm";
   div.style.position = "absolute";
   
   document.body.appendChild(div);
   
   var rect =
      div.getBoundingClientRect();
   
   document.body.removeChild(div);
  
   var pixelRatio =
      window.devicePixelRatio;
      
   debug.add("pixelRatio", pixelRatio);
   
   var pixelWidth = 
      (rect.width / 10) * pixelRatio;
    
   var pixelHeight =
      (rect.height / 10) * pixelRatio;
   
   return {
      x: pixelWidth,
      y: pixelHeight
   }
   
}
