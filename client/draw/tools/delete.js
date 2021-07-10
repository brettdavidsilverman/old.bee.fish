class Delete extends ToolboxItem {

   constructor() {
      super(
         {
            topLeft: {x: 10, y: 10}
         }
      );
      

   }


   async click(point) {
      alert(point);
   }
}
