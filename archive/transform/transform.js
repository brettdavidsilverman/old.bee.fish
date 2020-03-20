function scale(event) {

    scaleWidth = window.innerWidth / document.body.offsetWidth;
    scaleHeight = window.innerHeight / document.body.offsetHeight;
    
    var scale;
    if (scaleHeight <= scaleWidth)
        scale = scaleHeight;
    else
        scale = scaleWidth;
    alert(scale);
    document.body.style.transform = "scale(" + scale + ")";
}


window.addEventListener("load", scale, false);
