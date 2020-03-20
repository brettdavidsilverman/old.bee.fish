function import(fromIframeId, scriptId) {

   var fromIframe = document.getElementById(fomrIframeId);
   
   var script = document.createElement("script");
   
   var scriptText = fromIframe.contentWindow.export(scriptId)
   script.innerText = scriptText;
   
   document.body.appendChild(script);
}

function export(id) {
   var script = document.getElementById(id);
   return script.innerText;
}


