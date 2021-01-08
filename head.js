window.onerror =
function (message, source, lineno, colno, error)
{

   
   if (!error)
      error = {}
   
   var stack = "";
   if (error.stack)
      stack = String(error.stack);
     
   if (message instanceof Object)
      message = JSON.stringify(message, null, "   ");
   
   var string = 
      [
         message,
         source,
         "line: " + lineno,
         "col: " + colno,
         stack
      ].join("\n");
      
   if (alert.silent)
      console.log(string);
   else
      alert(string);
   
};


var hostname =
   window.location.hostname;
   
var port =
   window.location.port;

if (hostname.endsWith("bee.fish")) {

   // bee.fish
   google();
}


function google()
{
   document.writeln(
      '<!-- Global site tag (gtag.js) - Google Analytics -->'
   );
   
   document.writeln(
      '<script async src="https://www.googletagmanager.com/gtag/js?id=G-BD4P7481RP"><\/script>'
   );
  
   window.dataLayer = window.dataLayer || [];
   
   function gtag()
   {
      dataLayer.push(arguments);
   }

   gtag('js', new Date());

   gtag('config', 'G-BD4P7481RP');
 
}

function require(src) {
   console.log("Must include '" + src + "'");
}

var module = {}