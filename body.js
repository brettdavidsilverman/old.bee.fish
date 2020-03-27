googleTrackerBody();

function googleTrackerBody(tag) {

   if (!tag)
      tag = window.googleTag;
      
   document.writeln(
      '<noscript>' +
         '<iframe src="https://www.googletagmanager.com/ns.html?id=' + tag + '" ' +
              'height="0" width="0" ' +
              'style="display:none;visibility:hidden">' +
         '</iframe>' +
      '</noscript>'
   );
}