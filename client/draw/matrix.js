var Matrix = DOMMatrix;
Matrix.prototype.toJSON =
   function() {
      var array = this.toFloat64Array();
      return array.encode();
   }
   
Matrix.fromJSON =
   function(encoded) {
      var array = Float64Array.decode(encoded);
      return Matrix.fromFloat64Array(array);
   }


   
/*
class Matrix extends DOMMatrix {

   constructor(input) {
      super(input)
   }
   
   fromMatrix(source) {
      this.m11 = source.m11;
      this.m12 = source.m12;
      this.m13 = source.m13;
      this.m14 = source.m14;
      this.m21 = source.m21;
      this.m22 = source.m22;
      this.m23 = source.m23;
      this.m24 = source.m24;
      this.m31 = source.m31;
      this.m32 = source.m32;
      this.m33 = source.m33;
      this.m34 = source.m34;
      this.m41 = source.m41;
      this.m42 = source.m42;
      this.m43 = source.m43;
      this.m44 = source.m44;
      return this;
   }

   copy() {
      var destination = new Matrix();
      destination.fromMatrix(this);
      return destination;
   }
   
   inverse() {
      var inverse = super.inverse();
      var matrix = new Matrix();
      return matrix.fromMatrix(inverse);
   }
   
   toJSON() {
      return this.toFloat64Array();
   }
}
*/
