class UserInput {

   #touchPoints = null;

   #startPoint = null;
   #penMoved = false;
   #drawing = false;
   
   constructor(element) {
      var longPressTimer = null;
      var longPressPoint = null;
      var userInput = this;
      
      setEvents(element);
      
      function setEvents(element) {
         element.ontouchstart = onstart;
         element.onmousedown  = onstart;

         element.ontouchmove = onmove;
         element.onmousemove = onmove;
         
         element.ontouchend    = onend;
         element.ontouchcancel = onend;
         element.onmouseup     = onend;
         element.onmouseleave  = onend;
      
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
            userInput.#touchPoints =
               [
                  getPoint(event, 0),
                  getPoint(event, 1)
               ];
            
            // cancel any selected 
            // drawings
            userInput.#drawing = false;
            
            // cancel long press timer
            clearLongPressTimer(
               "multiple"
            );
            
            return;
         }
         
            
         // cancel selected transform
         userInput.#touchPoints = null;

         // get the point and start
         // drawing
         var point = getPoint(
            event, 0
         );
   
         userInput.#startPoint =
            Point.copy(point);
         userInput.#drawing = true;
         userInput.#penMoved = false;
         
         userInput.penDown(point);
         
         // Start the timer for long
         // touch
         longPressTimer =
            window.setTimeout(
               onlongpress,
               Canvas.LONG_PRESS_TIME
            );
         longPressPoint =
            Point.copy(userInput.#startPoint);

      }
      
      function onmove(event) {

         event.preventDefault();
         
         if (userInput.#touchPoints) {

             // get the next two points
             // for translate/scale
             userInput.#touchPoints.push(
                getPoint(event, 0)
             );
         
             userInput.#touchPoints.push(
                getPoint(event, 1)
             );

             // perform the transform
             touchTransform();
      
             // shift the two points
             // to replace the first ones
             userInput.#touchPoints
                 .shift();
             
             userInput.#touchPoints
                 .shift();
      
         }
         else if (userInput.#drawing) {
      
            // get the next point
            // and move the pen to
            // there
            var point = getPoint(event, 0);
               
            userInput.movePen(point);
            
            // If we have moved greater
            // then 5 pix then
            // cancel the long press
            // timer
            if ( !userInput.#penMoved &&
                 Point.distance(
                    point,
                    userInput.#startPoint
                 ) > Canvas.MAX_MOVE )
            {
            
               clearLongPressTimer(
                  "move"
               );
               
               userInput.#penMoved = true;
            }
            
   
         }
      }

      function onend(event) {
         event.preventDefault();
      
         clearLongPressTimer("end");

         if (userInput.#touchPoints) {
            // stop transforming
            userInput.#touchPoints = null;
            userInput.endTouchTransform();
         }
         else if (userInput.#drawing) {
            
            var clicked = false;
            if (!userInput.#penMoved) {
               clicked =
                  userInput.click(
                     userInput.#startPoint
                  );
            }
            
            if (!clicked) {
               userInput.penUp();
            }
         
            // stop drawing
            userInput.#drawing = false;
         
         }
      
      }
      
      // mouse wheel used for zooming
      function onmousezoom(event) {
      
         event.preventDefault();
         
         var point = getPoint(event);
      
         userInput.transformScreenToCanvas(
            point
         );
    
         // Normalize wheel to +1 or -1
         var wheel = event.deltaY < 0 ?
            1 : -1;
         
         // Compute zoom factor
         var zoom = Math.exp(
            wheel * Canvas.ZOOM_INTENSITY
         );
      
         userInput.transform(
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
         userInput.#drawing = false;
 
         userInput.longPress(longPressPoint);
        
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
      

      function touchTransform() {
   
         // transform a copy of
         // points
      
         var transformPoints =
            userInput.#touchPoints.map(
               copyAndTransform
            );
      
         // points now has four points
         // Two points, from start fingers
         // and two points from moved
         // fingers
         var fromDistance =
            Point.distance(
               transformPoints[0],
               transformPoints[1]
            );
  
         var toDistance =
            Point.distance(
               transformPoints[2],
               transformPoints[3]
            );
   
         var scale =
            toDistance / fromDistance;
   
         var fromMiddle =
            Point.middle(
               transformPoints[0],
               transformPoints[1]
            );
 
         var toMiddle =
            Point.middle(
               transformPoints[2],
               transformPoints[3]
            );
   
         // perform the actual translate
         // and scale
         userInput.transform(
            fromMiddle,
            toMiddle,
            scale
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

      }
   
   }
   
}