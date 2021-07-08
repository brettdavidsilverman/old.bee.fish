class ToolBox extends Id {

   tools;
   
   constructor(input) {
      super(input ? input.id : {});
      
      if (input.tools == undefined)
         tools = [];
      else
         tools = input.tools;
   }
   
   toJSON()
   {
      var id = super.toJSON();
      return {
         id,
         tools
      }
   }
   
}