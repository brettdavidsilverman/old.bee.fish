class FlowTool extends ToolboxItem {

   constructor(input) {
      super(input);

      this.label = "⬊";

   }

   async draw(context) {

      await super.draw(context);

      var dim = this.dimensions;

      context.lineWidth = 1;
      context.fillStyle = "black";
      context.textAlign    = 'center';
      context.textBaseline = 'middle';
      context.font = "40px Courier new";

      var point  = {
         x: dim.min.x + dim.width / 2,
         y: dim.min.y + dim.height / 2
      }

      context.fillText(this.label, point.x, point.y);
   }

   async click(point) {
      
      var flowTool = this;
      
      var selection = this.selection;

      console.log("Select next item to flow to...");

      var saveClick = this.toolbox.click;

      this.toolbox.click = async function(point) {

         var hit  = await this.parent.hitTest(
            point
         );


         if (hit && selection && hit != selection)
         {
            join(selection, hit);
         }

         this.click = saveClick;
         
         this.remove();
         this.parent.draw();

      }

      function join(fromItem, toItem) {
         var connector = 
            new Connector(
               {
                  from: fromItem,
                  to: toItem
               }
            );

         fromItem.outputs.push(connector);
         connector.save();
         fromItem.save();
      }

   }

}
