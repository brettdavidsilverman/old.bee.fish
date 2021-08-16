class IfConnectorTool extends ConnectorTool {

   form;

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "black";
      context.strokeStyle = "black";

      this.drawText(context, "", 40, "Courier New", "⬊", true, true);

      context.fillStyle = "green";
      context.strokeStyle = "green";

      this.drawText(context, "Bold", 40, "Courier New", "?", true, true);

   }

   createConnector(from, to) {
      return new IfConnector(
         {
            connector: {
               from,
               to
            }
         }
      );

   }

   async click(point) {
      if (  this.selection instanceof Connector &&
            !(this.selection instanceof IfConnector ) ) {

         if (confirm("Convert connector to conditional?")) {
            var connector = this.selection;
            var newConnector = new IfConnector(
               {
                  connector
               }
            );
            var parent = connector.parent;
            parent.children.push(newConnector);
            connector.remove();
            newConnector.save();
            parent.save();
            newConnector.selected = true;
            this.toolbox.parent.selection = newConnector;
            this.toolbox.parent.draw();
         }

         return;
      }
      else {
         return super.click(point);
      }
   }

}
