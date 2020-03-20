
function load(event) {
    
    if (window.location.hash)
        hashChange(event);
    
}

// The hash of the url specifies the id of the sub prpcess.
// Make this process full screen 
// If another sub process had fullscreen, revert it to normal
function hashChange(event) {

    // Check previous full screen
    oldHash = window.location.oldHash;
    
    if (oldHash) {
        oldSubProcess = document.querySelector(oldHash);
        if (oldSubProcess)
            toggleFullScreen(oldSubProcess);
    }
    
    window.location.oldHash = window.location.hash;
    
    
    // Check new full screen
    newHash = window.location.hash;
    
    if (newHash) {
        newSubProcess = document.querySelector(newHash);
    
        if (newSubProcess)
            toggleFullScreen(newSubProcess);
    }

}

// Changes a sub process from full screen to normal or vise versa
function toggleFullScreen(subProcess) {
    if (subProcess.fullScreen) {
        subProcess.fullScreen = false;
        subProcess.className = "subProcess";
        subProcess.style.position = "";
        subProcess.style.left =  "";
        subProcess.style.top = "";
        subProcess.style.width = "";
        subProcess.style.height = "";
       // subProcess.style.zIndex = "";
    }
    else {
        subProcess.fullScreen = true;
        subProcess.className = "subProcess fullScreen";
        subProcess.style.position = "absolute";
        subProcess.style.left = "0px";
        subProcess.style.top = "0px";
        subProcess.style.width = "100%";
        subProcess.style.height = "100%";

    }
    
}

// Register events 

// -Hash change (window navigation)
window.addEventListener("hashchange", hashChange, true);

// -Load a window
window.addEventListener("load", load, true);