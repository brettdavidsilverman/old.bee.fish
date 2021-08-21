class IfConnector extends Connector {
   form;
   width = 100;
   height = 100;
   
   constructor(input) {
      super(input ? input.connector : null);

      if (input.form){
         this.form = new Form(input.form);
      }
      else {
         var center = this.dimensions.center;
         var dimensions = new Dimensions(
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
            {
               form: {
                  html: "true",
                  borderColor: "orange",
                  line: {
                     strokeStyle: "orange",
                     item: {
                        dimensions
                     }
                  }
               }
            }
         );
         
         form.points = this.getPoints(form.dimensions);
         this.dimensions = this.dimensions.include(form.dimensions);
         delete form.label;
         this.form = form;
      }

      
   }

   toJSON() {
      return {
         connector: super.toJSON(),
         form: this.form
      }
   }

   async hitTest(point) {
      var hit = await this.form.hitTest(point);
      if (hit)
         return hit;
      return super.hitTest(point);
   }

   async draw(context) {
      var draw = await super.draw(context);
      if (draw) {
         await this.form.draw(context);
      }

      return draw;
   }

   async getSecondColor() {
      var form = this.form;
      var value = form.value;
      if (value == undefined)
         return "orange";
      else if (value == true)
         return "green";
      else
         return "red";
   }

   getPoints(dimensions) {
      var dim = dimensions;
      var points = new Points(
         {
            x: dim.left + dim.width / 2,
            y: dim.top
         },
         {
            x: dim.left + dim.width,
            y: dim.top + dim.height / 2
         },
         {
            x: dim.left + dim.width / 2,
            y: dim.top + dim.height
         },
         {
            x: dim.left,
            y: dim.top + dim.height / 2
         },
         {
            x: dim.left + dim.width / 2,
            y: dim.top
         }
      )
      return points;
   }

  
}