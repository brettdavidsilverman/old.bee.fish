function Indent(input, string) {

   if (typeof input == "number")
      this.tabCount = input;
   else if (typeof input == "object") {
      this.tabCount = input.tabCount;
      string = input.string;
   }
   
   var indent = "";
   for (var i = 0; i < this.tabCount; i++) {
      indent += Indent.tab;
   }
      
   this.addTabs = function(string) {
      var lines = string.split("\n");
      for (var index in lines) {
         lines[index] = indent + lines[index];
      }
      return lines.join("\n");
   }
   
   if (string != undefined)
      string = this.addTabs(string.toString());
   else
      string = "";
      
   if (!string.endsWith("\n"))
      string += "\n";
      
   this.toString = function() {
      return string;
   }
}

Indent.tab = "   ";