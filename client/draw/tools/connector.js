class ConnectorTool extends ToolboxItem {

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

      console.log("Select item to connect to...");

      var saveClick = this.toolbox.click;

      this.toolbox.click = async function(point) {

         var hit  = await this.parent.hitTest(
            point
         );


         if (hit && selection && hit != selection)
         {
            if (confirm("Join " + selection + " to " + hit + "?"))
               join(selection, hit);
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
         from.parent.children.push(connector);
         connector.save();
         from.parent.save();
         from.outputs.push(to)
         to.inputs.push(from);
         from.save();
         to.save();
      }

   }

}
