function script() {
alert("$");
return;
    document.write(".");
    var src = "file://sdcard/documents/addScript.js";
    document.write("<script src='" + src + "'><\/script>");
}

window.setTimeout(script, 0);