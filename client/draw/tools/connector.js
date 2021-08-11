class ConnectorTool extends ToolboxItem {

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "black";
      context.strokeStyle = "black";

      this.drawText(context, "40px Courier New", "⬊", true);

   }

   async click(point) {
      
      var flowTool = this;
      
      var selection = this.selection;

      console.log("Select item to connect to...");

      var saveClick = this.toolbox.click;

      this.toolbox.click = async function(point) {

         var hit  = await this.parent.hitTest(
            point
         );


         var from = selection;
         var to = hit;

         if (from && to && from != to)
         {
            if (confirm("Join " + from + " to " + to + "?"))
               join(from, to);
         }

         this.click = saveClick;
         
         this.remove();
         this.parent.draw();

      }

      function join(from, to) {
      
         var connector = 
            new Connector(
               {
                  from,
                  to
               }
            );
         connector.save();
         from.parent.children.push(connector);
         from.parent.save();
         from.outputConnectors.push(connector);
         from.outputs.push(to);
         to.inputConnectors.push(connector);
         to.inputs.push(from);
         from.save();
         to.save();
      }

   }

}
