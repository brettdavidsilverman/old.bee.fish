class IfConnector extends Connector {
   form;

   constructor(input) {
      super(input ? input.connector : null);
   }

   toJSON() {
      return {
         connector: super.toJSON(),
         form: this.form
      }
   }
}