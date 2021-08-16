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

}
