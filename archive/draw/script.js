// obsolete
/*
setupScript("../debug/script.js");
setupScript("../calculate/script.js");
setupScript("../indent/script.js");
setupScript("../treetraversal/script.js");
setupScript("../encode/script.js");
setupScript("matrix.js");
setupScript("matrix2d.js");
setupScript("style.js");
setupScript("events.js");
setupScript("touchevents.js");
setupScript("point.js");
setupScript("line.js");
setupScript("dimensions.js");
setupScript("canvas.js");
setupScript("codeeditor.js");
setupScript("drawing.js");
setupScript("buttons.js");

var debug;
var codeEditor;

window.addEventListener("load",
   function() {

   debug = setupDebug();
   debug.add("Hello");

   codeEditor = setupCodeEditor();

   function setupDebug() {

      var debug = new Debug(
         {
            id: "debug",
            displayCount: 10,
            filter: []
         }
      );
   
      debug.add(
         "Instructions",
         "One finger to draw.\n" +
         "Two fingers to scroll and zoom.\n" +
         "Tap a drawing to run its code.\n" +
         "Tap and hold to edit code."
      );

      return debug;
   
   }

   function setupCodeEditor() {

      var codeEditor = new CodeEditor(
         {
            id: "codeEditor",
            UIEvents: TouchEvents,
            canvas: canvas
         }
      );
   
      return codeEditor;
   }

}
);

function setupScript(src) {
   var script = document.createElement(
      "script"
   );
   script.src = src;
   document.head.appendChild(script)
}

*/
