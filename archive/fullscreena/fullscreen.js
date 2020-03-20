function properties(obj) {
     var ps = [];
     for (var p in obj) {
         ps.push(p);
     }
     ps.sort();
     document.write(ps.join("<br \>"));
}

function allowFullScreen(query) {

    // if (window.parent != window)
    //    return;

    
    items = document.querySelectorAll(query);

    for (index = 0; index < items.length; index++) {
        item = items.item(index);
        item.link = document.createElement("a");
        item.link.className = "fullScreenMask";
        item.link.href = "#" + item.id; document.body.insertBefore(item.link, item);

        item.className = "scale"; item.addEventListener("click",
            function (event) {
               window.location.assign("#" + item.id);
            },
            false
        );
        
    }
    
}


function hashChange(event) {
    oldHash = window.location.oldHash;
    
    if (oldHash) {
        oldIframe = document.querySelector(oldHash);
        if (oldIframe)
            toggleFullScreen(oldIframe);
    }
    
    window.location.oldHash = window.location.hash;
    
    newHash = window.location.hash;
    
    if (newHash) {
        newIframe = document.querySelector(newHash);
    
        if (newIframe)
            toggleFullScreen(newIframe);
    }
}


window.addEventListener("hashchange", hashChange, false);

function toggleFullScreen(iframe) {
    if (iframe.fullScreen == undefined) {
        iframe.link.className = iframe.className = "fullScreen";
        iframe.fullScreen = true;
    } else {
        iframe.link.className = iframe.className = "scale";
        iframe.fullScreen = undefined;
    }

}

window.addEventListener("load", hashChange, false);
