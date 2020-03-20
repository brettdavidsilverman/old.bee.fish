var okStyle = Style.buttons.ok;
var cancelStyle =
   Style.buttons.cancel;
var saveStyle = Style.buttons.save;
var loadStyle = Style.buttons.load;
var codeEditor = {}

var buttons = {
   index: [
      //"save",
      //"load",
      "ok",
      "cancel"
   ],
   save: {
      label: "Save",
      code: [
         "canvas.save()"
      ],
      visible: true,
      lines: [{
         start: {
            x: saveStyle.left,
            y: saveStyle.top
         },
         points: [
         {
            x: saveStyle.left +
               saveStyle.width,
            y: saveStyle.top
         },
               
         {
            x: saveStyle.left +
               saveStyle.width,
            y: saveStyle.top +
               saveStyle.height
         },
               
         {
            x: saveStyle.left,
            y: saveStyle.top +
               saveStyle.height
         },
               
         {
            x: saveStyle.left,
            y: saveStyle.top
         }
         ]
      }]
   
   },
   load: {
      label: "Load",
      code: [
         "canvas.load()"
      ],
      visible: true,
      lines: [{
         start: {
            x: loadStyle.left,
            y: loadStyle.top
         },
         points: [
         {
            x: loadStyle.left +
               loadStyle.width,
            y: loadStyle.top
         },
         {
            x: loadStyle.left +
               loadStyle.width,
            y: loadStyle.top +
               loadStyle.height
         },
         {
            x: loadStyle.left,
            y: loadStyle.top +
               loadStyle.height
         },
         {
            x: loadStyle.left,
            y: loadStyle.top
         }
         ]
      }]
   },
   ok: {
      label: "ok",
      visible: true,
      level: 1,
      getMatrix: function() {
         canvas.identity;
      },
      lines: [{ 
         start: {
            x: okStyle.left,
            y: okStyle.top
         },
         points: [
         {
            x: function() {
                  var x =
                     okStyle.left +
                     okStyle.width;
                  return x;
               },
            y: okStyle.top
         },
         {
            x: function() {
                  var x =
                     okStyle.left +
                     okStyle.width;
                  return x;
               },
            y: function() {
                  var y =
                     okStyle.top +
                     okStyle.height;
                  return y;
               }
         },
         {
            x: okStyle.left,
            y: function() {
                  var y =
                     okStyle.top +
                     okStyle.height;
                  return y;
               }
         },
         {
            x: okStyle.left,
            y: okStyle.top
         }
      ]
      }
   ]},
   cancel: {
      label: "cancel",
      visible: true,
      level: 1,
      getMatrix: function() {
         return canvas.identity;
      },
      lines: [
      { 
         start: {
            x: cancelStyle.left,
            y: cancelStyle.top
         },
         points: [{
            x: function() {
               var x = 
                  cancelStyle.left +
                  cancelStyle.width;
               return x;
            },
            y: cancelStyle.top
         },
         {
            x: function() {
               var x =
                  cancelStyle.left +
                  cancelStyle.width;
               return x;
            },
            y: function() {
               var y = 
                  cancelStyle.top +
                  cancelStyle.height;
               return y;
            },
         },
         {
            x: cancelStyle.left,
            y: function() {
                  var y = 
                     cancelStyle.top +
                     cancelStyle.height;
                  return y;
               }
         },
         {
            x: cancelStyle.left,
            y: cancelStyle.top
         }
      ]}
   ]}
}




