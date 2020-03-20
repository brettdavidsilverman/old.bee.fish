function toggleDisplay(id) {
   var element = document.getElementById(id);
   var style = element.style;
   if (style.display == "block")
      style.display = "none";
   else if (style.display == "none")
      style.display = "block";
   else
      style.display = "none";
}