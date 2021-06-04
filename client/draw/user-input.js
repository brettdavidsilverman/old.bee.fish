class UserInput extends Id {

   _touchPoints = null;

   _startPoint = null;
   _penMoved = false;
   _drawing = false;
   _touchCount = 0;
   
   constructor(element, input) {
      super(input);  

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
         
         userInput._touchCount = 1;
         
         if (event.touches)
            userInput._touchCount =
               event.touches.length;
         
         if (userInput._touchCount == 2) {
            // two or more fingers,
            // start scale/translate
            // mode
            userInput._touchPoints =
               [
                  getPoint(event, 0),
                  getPoint(event, 1)
               ];
            
            // cancel any selected 
            // drawings
            userInput._drawing = false;
            
            // cancel long press timer
            clearLongPressTimer(
               "multiple"
            );
            
            return;
         }
         
            
         // cancel selected transform
         userInput._touchPoints = null;

         // get the point and start
         // drawing
         var point = getPoint(
            event, 0
         );
   
         userInput._startPoint =
            point.copy();
         userInput._drawing = true;
         userInput._penMoved = false;
         
         userInput.penDown(point);
         
         // Start the timer for long
         // touch
         longPressTimer =
            window.setTimeout(
               onlongpress,
               Canvas.LONG_PRESS_TIME
            );
         longPressPoint =
            userInput._startPoint.copy();

      }
      
      function onmove(event) {

         event.preventDefault();
         
         if (userInput._touchCount == 2) {

             // get the next two points
             // for translate/scale
             userInput._touchPoints.push(
                getPoint(event, 0)
             );
         
             userInput._touchPoints.push(
                getPoint(event, 1)
             );

             // perform the transform
             touchTransform();
      
             // shift the two points
             // to replace the first ones
             userInput._touchPoints
                 .shift();
             
             userInput._touchPoints
                 .shift();
      
         }
         else if (userInput._drawing) {
      
            // get the next point
            // and move the pen to
            // there
            var point = getPoint(event, 0);
               
            userInput.movePen(point);
            
            // If we have moved greater
            // then 5 pix then
            // cancel the long press
            // timer
            if ( !userInput._penMoved &&
                 Point.distance(
                    point,
                    userInput._startPoint
                 ) > Canvas.MAX_MOVE )
            {
            
               clearLongPressTimer(
                  "move"
               );
               
               userInput._penMoved = true;
            }
            
   
         }
      }

      function onend(event) {
         event.preventDefault();
      
         clearLongPressTimer("end");

         if (event.touches)
            userInput._touchCount =
               event.touches.length;
         else
            userInput._touchCount = 0;
            
         if (userInput._touchPoints &&
             userInput._touchCount == 0) {
            // stop transforming
            userInput._touchPoints = null;
            userInput.endTouchTransform();
         }
         else if (userInput._drawing) {
            
            var clicked = false;
            if (!userInput._penMoved) {
               clicked =
                  userInput.click(
                     userInput._startPoint
                  );
            }
            
            if (!clicked) {
               userInput.penUp();
            }
         
            // stop drawing
            userInput._drawing = false;
         
         }
      
      }
      
      // mouse wheel used for zooming
      function onmousezoom(event) {
      
         event.preventDefault();
         
         var point = getPoint(event);
      
         point = userInput.creenToCanvas(
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
         userInput._drawing = false;
 
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
   
         // transform and copy
         // points
         var canvasPoints =
            userInput._touchPoints.map(
               screenPoint => 
                  userInput
                     .screenToCanvas(
                        screenPoint
                     )
            );
            
         // points now has four points
         // Two points, from start fingers
         // and two points from moved
         // fingers
         var fromDistance =
            Point.distance(
               canvasPoints[0],
               canvasPoints[1]
            );
  
         var toDistance =
            Point.distance(
               canvasPoints[2],
               canvasPoints[3]
            );
   
         var scale =
            toDistance / fromDistance;
   
         var fromMiddle =
            Point.middle(
               canvasPoints[0],
               canvasPoints[1]
            );
 
         var toMiddle =
            Point.middle(
               canvasPoints[2],
               canvasPoints[3]
            );
   
         // perform the actual translate
         // and scale
         userInput.transform(
            fromMiddle,
            toMiddle,
            scale
         );
     

      }
   
   }
   
}