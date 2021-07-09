class ToolBox extends Id {

   tools;
   
   constructor(input) {
      super(input ? input.id : {});
      
      if (input == undefined)
         input = {};
         
      if (input.tools == undefined)
         this.tools = [
            new Pointer(new Delete())
         ];
      else
         this.tools = input.tools;
   }
   
   toJSON()
   {
      var id = super.toJSON();
      return {
         id,
         tools: this.tools
      }
   }
   
}