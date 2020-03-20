var Style = {}
var canvas = {}
Style.gap = 5;

Style.buttons = {}
Style.buttons.left =
   Style.gap;
Style.buttons.top = 160;
Style.buttons.width = 80;
Style.buttons.height = 40;
Style.buttons.label = {}
Style.buttons.label.top = 5;
Style.buttons.label.left = 5;
Style.buttons.label.height = 15;
Style.buttons.label.gap = Style.gap;
Style.buttons.label.font = {}
Style.buttons.label.font.name =
   "Courier New";
Style.buttons.label.font.size = 8;
Style.buttons.load = {
      
   left:   Style.buttons.left,
   top:    Style.buttons.top,
   width:  Style.buttons.width,
   height: Style.buttons.height

}

Style.buttons.save = {
   left:   Style.gap,
   top:    Style.buttons.top +
           Style.buttons.height +
           Style.gap,
   width:  Style.buttons.width,
   height: Style.buttons.height
}

Style.buttons.ok = {
   left: function() {
   
      var left;

      if (canvas.orientation ==
          "portrait")
         left = Style.gap;
      else
         left = canvas.scaleWidth -
            Style.buttons.ok.width -
            Style.gap;
            
      return left;
   },
   top:    Style.gap,
   width:  Style.buttons.width,
   height: Style.buttons.height
}
     
Style.buttons.cancel = {
        
   left: function() {
      var left;
      
      if (canvas.orientation ==
          "portrait")
         left = Style.buttons.ok.left +
            Style.buttons.ok.width +
            Style.gap;
      else
         left = Style.buttons.ok.left;

      return left;
   },
   top: function() {
      var top;
      
      if (canvas.orientation ==
          "portrait")
         top = Style.gap;
      else
         top = Style.buttons.ok.top +
            Style.buttons.ok.height +
            Style.gap * 2;
      return top;
   },
   width: Style.buttons.width,
   height: Style.buttons.height
}

// code editor
Style.codeEditor = {
   borderWidth: 1,
   top: function() {
      var top;
      if (canvas.orientation ==
          "portrait")
         top = Style.buttons.height +
               Style.gap * 2;
      else
         top = Style.gap;
               
      return top;
   },
   left: Style.gap,
   width: function() {
      var width;
     
      if (canvas.orientation == 
          "portrait")
         width = canvas.scaleWidth -
            Style.gap * 2 -
            Style.codeEditor.borderWidth * 2;
      else
         width = canvas.scaleWidth -
            Style.buttons.ok.width -
            Style.codeEditor.borderWidth * 2 -
            Style.gap * 3;
                 
      return width;
   },
   height: function() {
      var height;
      if (canvas.orientation ==
          "portrait")
         height = canvas.scaleHeight -
            Style.buttons.ok.height -
            Style.gap * 3 -
            Style.codeEditor.borderWidth * 2;
      else
         height = canvas.scaleHeight -
            Style.gap * 2 -
            Style.codeEditor.borderWidth * 2;
            
      return height;
           
   }
}


Style.applyToElement = function(input) {
   var element = input.element;
   var style = input.style;
   
   element.style.left =
      style.left + 
      "px";
   
   element.style.top = 
      style.top +
      "px";
      
   element.style.width =
      style.width +
      "px";
      
   element.style.height =
      style.height + 
      "px";
   
   if (!style.borderWidth)
      element.style.border = "0";
   else
      element.style.border =
         style.borderWidth + 
         "px solid";
    
}
