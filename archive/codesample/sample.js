<!DOCTYPE html>
<html>
   <head>
      <meta name="viewport" content="width=device-width, initial-scale=1"></meta>
      <script>
window.onerror = function(message, source, line) {
   alert(message + "\n" + line);
}

function properties(obj) {
    var ps = [];
    for (var p in obj) {
        ps.push(p);
    }
    ps.sort();
    document.write(ps.join("<br \>"));
}
      </script>
   </head>
   <body>
      <h1>Code Sample JS</h1>
      <script>
var blocks = {

   "start" : function () {

      var name = document.getElementById("name");
      
      name.onfocus = function() {
         blocks.process("start");
      }
      
      var go = document.getElementById("go");
      
      go.onclick = function() { blocks.process("go");
      }
      
      input = {
         name : name,
         go : go
      }

      input.name.focus();
       
      return EXIT;
   },
   
   "go" : {
      "type" : "assignment",
      "from" : "input.name.value",
      "to" : "input",
      "next" : "check"
   },
   
   "check" : {
      "type" : "condition",
      "condition" : {
         "type" : "equality",
         "left hand side" : "input",
         "right hand side" : "''"
      }
      "true" : "failure";
      "false" : "success";
   },
   
   "failure" : {
      "type" : "call function",
      "call function" : {
         "function" : "alert",
         "parameters" : [
             {
                "type" : "string",
                "value" : "Everyone has a name"
             }
         ]
      },
      "next" : "start"
   },
   
   "success" : {
      "type" : "call function",
      "call function" : {
         "function" : "alert",
         "parameters" : [
            {
               "type" : "join strings" {
                  "first" : {
                     "type" : "string",
                     "string" : "Thank you"
                  },
                  "second" : {
                     "type" : "variable",
                     "variable" : "input"
                  }
               }
            }
         ]
      },
      "next" : "EXIT";
   }
}
   


window.onload = function(event) {
   blocks.process("start");
}

      </script>
   </body>
</html>