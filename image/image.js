HTMLImageElement.fromJSON =
function(json, memory) {
   var width = json.width;
   var height = json.height;
   var image = new Image(width, height);
   Object.assign(image, json);
   return image;
}

Image.prototype.toString =
function(shorthand = Shorthand.HUMAN) {

   new Shorthand(shorthand);
   
   var json = {
      width:  this.width,
      height: this.height,
      src:    this.src
   }

   Object.assign(json, this);

   var string = JSON.stringify(
      json,
      null,
      "   "
   );
   
   return string;
}
