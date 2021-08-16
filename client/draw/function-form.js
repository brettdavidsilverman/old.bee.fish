class FunctionForm extends Form {
   constructor(input) {
      super(input);
   }

   toJSON() {
      return {
         form: super.toJSON()
      }
   }
}