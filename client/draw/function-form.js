class FunctionForm extends Form {

   constructor(input) {
      super(input ? input.form : null);
   }

   toJSON() {
      return {
         form: super.toJSON()
      }
   }


}