
function toggleFullScreen() {

   var fullScreenElement = getFullScreenElement();
   
   if (fullScreenElement == null) {
      requestFullScreen(document.documentElement);
   }
   else {
      exitFullScreen();
   }
}

// get the current full screen element
function getFullScreenElement() {
   var fullScreenElementProperty = getProperty(document,
      [
         "fullScreenElement",
         "fullscreenElement",
         "mozFullScreenElement",
         "mozFullscreenElement",
         "msFullScreenElement",
         "msFullscreenElement",
         "webkitFullScreenElement",
         "webkitFullscreenElement"
      ]
   );

   var fullScreenElement = null;
   if (fullScreenElementProperty)
      fullScreenElement = document[fullScreenElementProperty];
   return fullScreenElement;
}

// request full screen
function requestFullScreen(element) {

   var requestFullScreenProperty = getProperty(element,
      [
         "requestFullScreen",
         "requestFullscreen",
         "mozRequestFullScreen",
         "mozRequestFullscreen",
         "msRequestFullScreen",
         "msRequestFullscreen",
         "webkitRequestFullScreen",
         "webkitRequestFullscreen"
      ]
   );
   if (requestFullScreenProperty) {
      element[requestFullScreenProperty]();
   }
}

// exit full screen
function exitFullScreen() {
     
   var exitFullScreenProperty = getProperty(document,
      [
         "exitFullScreen",
         "exitFullscreen",
         "mozExitFullScreen",
         "mozExitFullscreen",
         "mozCancelFullScreen",
         "mozCancelFullscreen",
         "msExitFullScreen",
         "msExitFullscreen",
         "webkitExitFullScreen",
         "webkitExitFullscreen"
      ]
   );
      
   if (exitFullScreenProperty)
      document[exitFullScreenProperty]();

}

/*

// capture the full screen change event
// to toggle the full screen class
document.addEventListener("fullscreenchange", setFullScreenClass, true);
document.addEventListener("ofullscreenchange", setFullScreenClass, true);
document.addEventListener("msfullscreenchange", setFullScreenClass, true);
document.addEventListener("mozfullscreenchange", setFullScreenClass, true);
document.addEventListener("webkitfullscreenchange", setFullScreenClass, true);

*/

// different browsers have differently named properties
// Find a property in obj from the array properties
// Return property name or null if not found.
function getProperty(obj, properties) {
   for (var index = 0; index < properties.length; index++) {
      var property = properties[index];
      if (property in obj) {
         return property;
      }
   }
   
   return null;
}
