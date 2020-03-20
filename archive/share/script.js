// Request a script from an iframe
// and of successful, sdd the script
function importScript(input) {
 
   var iFrame = document.getElementById(input.iFrameId);
   
   var data = input;
   data.type = "requestScript";
   
   iFrame.contentWindow.postMessage(
      data,
      "*"
   );
   

}

// Listen to the message event,
// handling two types of messages,
// rewuest and response.
// Request posts a message requesting a script.
// Response adds the script to the page.
window.addEventListener(
   "message",
   function(event) {
   
      var data = event.data;
      
      switch (data.type) {
      case "requestScript":
         postScriptResponse(data);
         break;
      case "responseScript":
         addScript(data);
         break;
      default:
         throw "Unknown message data type '" + data.type + "' in share.js.";
      }
      
   }
);

// Send a message to the iframe
// requesting a script
function postScriptResponse(input) {

   var script = document.getElementById(
      input.remoteScriptId
   );
   
   window.parent.postMessage(
      {
         type: "responseScript",
         script: script.innerHTML,
         request: input
      },
      "*"
   );

}

// Add a script to the windoe that
// requested the script from an iframe
function addScript(input) {

   // the original request
   var request = input.request;
   
   var script = document.createElement("script");
   
   var text = document.createTextNode(
      input.script
   );
   
   script.appendChild(text);
   
   if (request.localScriptId)
      script.id = request.localScriptId;
      
   var parent = null;
   
   if (request.localParentId)
      parent = document.getElementById(request.localParentId);
      
   if (!parent)
      parent = document.body;
   
   parent.appendChild(script);
}
