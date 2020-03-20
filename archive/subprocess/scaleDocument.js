function scaleDocument(data) {

   // Get the parent window size
   parentSize = {
       width: data.parentWindow.innerWidth,
       height: data.parentWindow.innerHeight
   }
   
   // Get our (child) size
   size = {
       width: window.innerWidth,
       height: window.innerHeight
   }
   
   // calculate the scale
   scale = {
        x : size.width / parentSize.width,
        y : size.height / parentSize.height
    }
   
    // scale to smallest
    var smallScale;

    if (data.child.fullScreen)
        smallScale = null;
    else
        if (scale.x < scale.y)
            smallScale = scale.x;
        else
            smallScale = scale.y;

    
    if (smallScale)
        cssScale = "scale(" + smallScale + ", " + smallScale + ")";
    else
        cssScale = null;
    
    document.body.style.transform = 
        document.body.style.webkitTransform = cssScale;

}


