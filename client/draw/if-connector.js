class IfConnector extends Connector {
   form;
   width = 100;
   height = 100;

   constructor(input) {
      super(input ? input.connector : null);

      if (input.form){
         this.form = new ChildPointer({parent: this, pointer: {key: input.form}});
      }
      else {
         input = {};
         input.form = {}
         input.form.html = "return true;";
         input.form.item = {};
         var center = this.dimensions.center;
         input.form.item.dimensions = new Dimensions(
            {
               min: {
                  x: center.x - this.width / 2,
                  y: center.y - this.height / 2
               },
               max: {
                  x: center.x + this.width / 2,
                  y: center.y + this.height / 2
               }
            }
         )
         var form = new FunctionForm(
            input
         );
         form.save();
         this.children.push(form);
         this.form = new ChildPointer({parent: this, pointer: {object: form}});
      }
      
   }

   toJSON() {
      return {
         connector: super.toJSON(),
         form: this.form
      }
   }

   async hitTest(point) {
      var form = await this.form.fetch();
      var hit = await form.hitTest(point);
      if (hit)
         return hit;
      return super.hitTest(point);
   }

   async getColor() {
      return "orange";
   }

}