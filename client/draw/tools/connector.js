class ConnectorTool extends ToolboxItem {

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "black";
      context.strokeStyle = "black";

      this.drawText(context, "", 40, "Courier New", "⬊", true, true);

   }

   async click(point) {
      
      var self = this;
      
      var selection = this.selection;

      if (!confirm("Select item to connect to"))
         return;

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
         
         this.parent.draw();

      }

      function join(from, to) {
      
         var connector = self.createConnector(from, to);

         connector.save();
         from.parent.children.push(connector);
         from.parent.save();
         from.outputConnectors.push(connector);
         from.outputs.push(to);
         to.inputConnectors.push(connector);
         to.inputs.push(from);
         from.save();
         to.save();
         self.toolbox.parent.selection.selected = false;
         self.toolbox.parent.selection = connector;
         connector.selected = true;
      }

   }

   createConnector(from, to) {
      return new Connector(
         {
            from,
            to
         }
      );
   }

}
