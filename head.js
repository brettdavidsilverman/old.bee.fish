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
      
   alert(
      [
         message,
         source,
         "line: " + lineno,
         "col: " + colno,
         stack
      ].join("\n")
   );
};



var hostname =
   window.location.hostname;
   
var port =
   window.location.port;
   
if (port)
   hostname += ":" + port;

if (hostname.endsWith("fee.bee.fish")) {

   // fee.bee.fish
   googleTrackerHead(
      window.googleTag = 'GTM-KNTR9HG'
   );

}
else if (hostname.endsWith("bee.fish")) {

   // bee.fish
   googleTrackerHead(
      window.googleTag = "GTM-TQLM7H5"
   );

}


function googleTrackerHead(tag) {
   (function(w,d,s,l,i){w[l]=w[l]||[];w[l].push({'gtm.start':
   new Date().getTime(),event:'gtm.js'});var f=d.getElementsByTagName(s)[0],
   j=d.createElement(s),dl=l!='dataLayer'?'&l='+l:'';j.async=true;j.src=
   'https://www.googletagmanager.com/gtm.js?id='+i+dl;f.parentNode.insertBefore(j,f);
   })(window,document,'script','dataLayer',tag);
}