/*
Add connections to touchEvents and redirect them to the canvas so that the user can generate drawings and
scale and translate
*/

function TouchEvents(input) {

   var touchEvents = this;
   
   var element = input.element;
   
   var elementRect = 
      element.getBoundingClientRect();
   
   var gesture = null;
   var time1Points = null;
   var time2Points = null;
   var startPoint = null;
   var point = null;
   var waitToStartOver = false;
   var timerId = 0;
   var pixPerMm =
      input.pixelsPerMillimeter;
   var pixelRatio =
     window.devicePixelRatio;
     
   const moveDistance =
      TouchEvents.moveDistance;
      
   declareEvents();
   
   // outgoing events
   function declareEvents() {
   
      touchEvents.onclick =
         function(point) {}
      
      touchEvents.oncontextmenu =
         function(point) {}
      
      touchEvents.onpendown =
         function(point) {}
      
      touchEvents.onpenup =
         function(point) {}
      
      touchEvents.onmovepen =
         function(point) {}
         
      touchEvents.oncancelpen =
         function() {}
         
      touchEvents.ontransform =
         function(
            scale,
            time1Points, 
            time2Points
         ) {}

   }
   
   
   this.setGesture = function(newGesture) {
      if (gesture)
         element.classList.remove(gesture);
         
      gesture = newGesture;
      
      if (gesture)
         element.classList.add(gesture);

   }
   
   // set up incoming event listeners to element.touchEvents

   element.onclick =
      function(event) {
         event.preventDefault();
      }
      
   element.ondblclick = 
      function(event) {
         event.preventDefault();
      }
      
   element.oncontextmenu = 
      function(event) {
         event.preventDefault()
      }
      
   // prevent on context menu from
   // selecting text
   element.onselectstart = 
      function(event) {
         //if (this.isContentEditable)
            event.preventDefault();
      }

   
   // On touch start
   element.ontouchstart = function (event) {
      event.preventDefault();
      
      var touchCount =
         event.touches.length;
      
      var touch = event.touches[0];
      
      point = startPoint
            = getPoint(touch);
      
      if (waitToStartOver)
         return;
     
      if (touchCount == 1)
         timerId = window.setTimeout(
            function() {
               touchEvents
                  .oncontextmenu(
                     point
                  );
               waitToStartOver = true;
            },
            TouchEvents.contextTimer
         );

      switch (gesture) {
      case "pen":
         
         if (touchCount > 1) {
         
            touchEvents.setGesture(
               null
            );
            
            touchEvents.oncancelpen();
            waitToStartOver = true;
         }
         
         break;
            
      case null:
         
         if (touchCount == 2) {
            touchEvents.setGesture(
               "scroll"
            );
            
            time2Points = getPoints(
               event.touches
            );
         }
         
         break;
      }
      
   }
   
   // set passive to false to make sure we can capture the event to cancel the default action of refreshing or scrolling
      
   element.addEventListener(
      "touchmove",
      function(event) {
         event.preventDefault();
      },
      {
         passive: false,
         capture: true 
      } 
   );
   
   
   element.ontouchmove = function(event) {
    
      event.preventDefault();
      
      point = getPoint(
         event.touches[0]
      );

      if (timerId) {
      
         var distance =
            Point.distance(
               startPoint,
               point
            );
         
         if (distance > 5) {
            window
            .clearTimeout(timerId);
            timerId = 0;
         }
      }
      
      var lastPoint = point;
      var touchCount =
         event.touches.length;
         
      
      if (waitToStartOver) {
         return;
      }

      switch (gesture) {
      case null:
         
         if (touchCount == 1) {
            // start new pen gesture
            touchEvents.setGesture(
               "pen"
            );
            touchEvents.onpendown(
               lastPoint
            );
         }
               
      case "pen":
         // move existing pen
         touchEvents.onmovepen(
            point
         );
         break;
      case "scroll":
         
         time1Points = time2Points;

         time2Points = getPoints(
            event.touches
         );
   
         if (time1Points) {
         
            var scale = getScale(
               time1Points,
               time2Points
            );
            
            // perform the transform
            touchEvents.ontransform(
               {
                  x: scale,
                  y: scale
               },
               time1Points,
               time2Points
            );
         

         }
      
         break;
      }
      
      function getScale(
         time1Points,
         time2Points) {

         var time1_Distance =
            Point.distance(
               time1Points[0],
               time1Points[1]
            );
         
         var time2_Distance = 
            Point.distance(
               time2Points[0],
               time2Points[1]
            );
         
         var scale =
            time2_Distance /
            time1_Distance;
            
         return scale;
         
      }

         
   }
  
   element.ontouchend =
   element.ontouchcancel = function(event) {
   
      event.preventDefault();
      
      var touchCount =
         event.touches.length;
           
      if (timerId) {
         window.clearTimeout(timerId);
         timerId = 0;
      }

      var distance =
         Point.distance(
            startPoint,
            point
         );
      
      if (distance < moveDistance &&
          touchCount == 0 &&
          !waitToStartOver)
      {
         touchEvents.onclick(point);
         touchEvents.setGesture(null);
      }
     
      if (touchCount == 0)
         waitToStartOver = false;
      else {
         touchEvents.setGesture(null);
         waitToStartOver = true;
      }
      
      switch (gesture) {
      case "scroll":
         touchEvents.setGesture(null);
         break;
      case "pen":
         touchEvents.onpenup(point);
         touchEvents.setGesture(null);
         break;

      default:
      }
         
   }


   function getPoint(touch) {
 
      // position within the element.
      var point = {
         x: (touch.clientX -          
             elementRect.left) /
             pixPerMm.x *
             pixelRatio,
         y: (elementRect.height -
               (touch.clientY - 
                elementRect.top)
             ) / pixPerMm.y
             * pixelRatio,
         time: new Date()
      }
      
      return point;
   }
      
   function getPoints(touches) {
   
      points = [];
      
      for (index = 0; index < touches.length; index++) {
         touch = touches[index];
         point = getPoint(touch);
         points.push(point);
      }

      return points;
      
   }
   
   this.resize = function(width, height) {
      // update elementRect
      elementRect = element.getBoundingClientRect();
      pixelRatio = window.devicePixelRatio;
      this.ontransform(
         time1Points,
         time2Points
      );

   }
   
   return this;
}

TouchEvents.moveDistance = 10;
TouchEvents.contextTimer = 500;