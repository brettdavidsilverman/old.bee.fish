HTMLImageElement.fromJSON = function(json, memory) {
   var image = new Image();
   image["="] = new Id(json.id);
   return image;
}
