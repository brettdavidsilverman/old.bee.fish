function Drawing(input) {

   var drawing = this;
   
   this.lines = [];
   if (input && input.lines)
      loadLines(input.lines);
   
   this.children = [];
   if (input && input.children)
      loadChildren(input.children);

   this.visible = input.visible;
   if (this.visible == undefined)
      this.visible = true;
      
   var parent;
   
   if (input && input.parent)
      parent = input.parent;
   else
      parent = null;
   
   if (input && input.id)
      this.id = input.id;
   else
      this.id = null;
      
   if (input && input.label)
      this.label = input.label;
   else
      this.label = this.id;
   
   if (input && input.level)
      this.level = input.level;
   else
      this.level = 0;
      
   if (input && input.getMatrix)
      this.getMatrix =
         input.getMatrix;
   else
      this.getMatrix =
         function(matrix) {
            return matrix;
         }
   
   if (input && input.dimensions)
      this.dimensions =
         input.dimensions;
   else
      this.dimensions =
         new Dimensions();
         
   this.loadCode = function(code) {
      if (Array.isArray(code))
         code = code.join(";");
         
      this.code = code;
      delete this.f;
      delete this.error;
      delete this.value;
      try {
         this.f = new Function(
            this.code
         );
         return true;
      }
      catch(error) {
         this.error = error;
         return false;
      }
   }
   
   if (input && input.code)
      this.loadCode(input.code);
   else
      this.code = null;

   function loadLines(lines) {
      lines.forEach(
         function(line) {
            drawing.lines.push(
               new Line(line)
            );
         }
      );
   }
   
   function loadChildren(children) {
     
      children.forEach(
      
         function(child) {
         
            child.parent = drawing;
            
            var childDrawing =
               new Drawing(child);
            
         }
         
      );
   }

   this.onaddline = function(line) {};
   this.onclick = function() {};
   this.oncontextmenu = function() {};
   this.ondraw = function() {};
   this.onruncode = function() {};
   
   this.toString = function() {
      return JSON.stringify(this);
   }
   
   this.draw = function(
      context
   ) {
   
      var matrix = this.getMatrix(context.matrix);
      
      matrix.applyToContext(context);
      
     // debug.add("drawing.draw", matrix);

      var drawDimensions = false;
      
      if (this.error) {
         context.strokeStyle = "red";
         context.fillStyle = "rgba(255, 0, 0, 0.2)";
         drawDimensions = true;
      }
      else if (this ==
               selectedDrawing)
      {
         context.strokeStyle = "blue";
         context.fillStyle = "rgba(0, 0, 255, 0.2)";
         drawDimensions = true;
      
         if (!this.code) {
            context.strokeStyle = "orange";
            context.fillStyle = "rgba(255, 165, 0, 0.2)";
         }
         else if (!this.error) {
            context.strokeStyle = "green";
            context.fillStyle = "rgba(0, 255, 0, 0.2)";
         }
      }
      
      if (drawDimensions)
         this.dimensions
            .draw(context);
      
      context.beginPath();
      
      this.lines.forEach(
         function(line) {
            line.draw(context);
         }
      );
    
      context.stroke();
      
      drawText(
         this.dimensions
      );
      
      
      this.ondraw(context);

   
      function drawText(dim) {
      
         context.fillStyle = "black";
         context.strokeStyle = "black";
      
         var fontSize =
            (Style.buttons
             .label.font.size);
              
         var font =
            fontSize + "px " +
            Style.buttons
            .label.font.name;
         
         context.font = font;
        
         var top =
            dim.top +
               Style.buttons
               .label.top +
               fontSize;
            
         var left =
            dim.left + Style.buttons.label.left;
         context.scale(1, -1);
         context.translate(0, -canvas.height);
         drawTextLine(
            "blue",
             drawing.label
         );
       
         drawTextLine(
            "green",
             drawing.value
         );
         
         drawTextLine(
            "black",
             drawing.code
         );
         
         drawTextLine(
            "red",
             drawing.error
         );
         
      
         function drawTextLine(
            color,
            text
          ) {
          
            if (isNull(text))
               return;
            
            text = new String(text);
            
            var texts  = text.split(
               "\n"
            );
            
            context.fillStyle =
               color;
               
            context.strokeStyle =
               color;
            
            for (var index in texts) {
             
               var textLine = texts[
                  index
               ];
               
               context.fillText(
                  textLine,
                  left,
                  top
               );
            
               context.strokeText(
                  textLine,
                  left,
                  top
               );
               
               top +=
                  Style.buttons
                  .label.gap +
                  fontSize;
            }
         }
      }
      
   }

   this.addLine = function(line) {
   
      this.lines.push(
         line
      );
      
      var lineDim =
         line
         .calculateDimensions();
        
      this.dimensions.min =
         Point.min(
            lineDim.min,
            this.dimensions.min
         );
      
      this.dimensions.max =
         Point.max(
            lineDim.max,
            this.dimensions.max
         );
         
      this.onaddline(line);
      
   }

   this.click = function(point) {
      this.runCode();
      selectedDrawing = this;
      this.onclick(point);
   }
   
   
   this.contextMenu = function(point) 
   {        
      selectedDrawing = this;
     
      this.codeEditor = new CodeEditor(
         {
            id: "codeEditor",
            UIEvents: TouchEvents,
            canvas: canvas,
            drawing: this
         }
      );

      this.oncontextmenu(point);
   }
   
      
   this.runCode  = function(point) {
      this.error = null;
      this.value = null;
      
      if (isNull(this.code))
         return true;
      
      if (typeof this.f != "function"
         && !this.loadCode(this.code))
            return false;
            
      try {
         this.value = this.f();
      }
      catch (err) {
         this.error = err;
         alert(err);
      }
      
      canvas.draw();
     
      this.onruncode();
      
      return true;
   }
   
   this.contains = function(drawing) {
      var dim = this.dimensions;
      containsDim = drawing.dimensions;
      return (dim.left <= containsDim.left
         && dim.right >= containsDim.right
         && dim.top <= containsDim.top
         && dim.bottom >= containsDim.bottom);
   }
   
   this.getParent = function() {
      return parent;
   }
   
   // set the new parent
   this.setParent = function(newParent) {
   
      var oldParent = parent;
         
      // remove the drawing from the 
      // old parents array of children
      if (oldParent) {
         var array = oldParent.children;
      
         var index = oldDrawings.indexOf(this);
      
         var head = oldDrawings.slice(0, index);

         var tail = oldDrawings.slice(index + 1);
      
         array = head.concat(tail);
      
         oldParent.children = array;
      }
      
      // add the drawing to its parent
      newParent.children.push(this);
         
      parent = newParent;
   }

   
   this.calculateDimensions =
      function() {
   
      drawing.dimensions =
         new Dimensions();
         
      var index;
      for (index in drawing.children)
      {
      
         var child =
            drawing.children[index];
         
         var childDim = child.calculateDimensions();
        
         drawing.dimensions.min =
            Point.min(
               childDim.min,
               drawing.dimensions.min
            );
      
         drawing.dimensions.max =
            Point.max(
               childDim.max,
               drawing.dimensions.max
            );
      }
      
      for (index in drawing.lines) {
         var line =
            drawing.lines[index];
            
         var lineDim =
            line
            .calculateDimensions();
        
         drawing.dimensions.min =
            Point.min(
               lineDim.min,
               drawing.dimensions.min
            );
      
         drawing.dimensions.max =
            Point.max(
               lineDim.max,
               drawing.dimensions.max
            );
      }
      
      return drawing.dimensions;
   }
   
   this.hitTest = function(point, matrix) {
   
      if (!Bool(this.visible))
         return false;
      
      var dim = this.dimensions;
      
      var matrix = this.getMatrix(matrix);
      
      point =
         matrix
         .inverse.transformPoint(
            point
         );
    
      return dim.isPointInside(point);
      
   }
   
   return this;
}
