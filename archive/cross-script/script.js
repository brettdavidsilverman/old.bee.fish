window.addEventListener("message", evalScript);

function getScript(iframe, id) {

   iframe
   .contentWindow
   .postMessage(
      {
         type: "request",
         id: id
      },
      "*"
   );
}

function evalScript(event) {
   var type = event.data.type;
   if (type == "request") {
      var id = event.data.id;
      var script = document.getElementById(id);
      var text = script.innerHTML;
      event.source.postMessage(
         {
            type: "response",
            id: id,
            text: text
         },
         "*"
      );
   }
   else if (type == "response") {
      var script =
         document
         .createElement("script");
         
      script.appendChild(
         document.createTextNode(
            event.data.text
         )
      );
      
      document.body.appendChild(script);
   }
}

