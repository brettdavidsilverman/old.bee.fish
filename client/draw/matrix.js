var Matrix = DOMMatrix;

Matrix.prototype.toJSON =
   function() {
       return this.toString();
   }

Matrix.fromJSON =
   function(string) {
       return new Matrix(string);
   }
/*
Matrix.fromMatrix =
   function(source) {

      var matrix = new Matrix();
      matrix.m11 = source.m11;
      matrix.m12 = source.m12;
      matrix.m13 = source.m13;
      matrix.m14 = source.m14;
      matrix.m21 = source.m21;
      matrix.m22 = source.m22;
      matrix.m23 = source.m23;
      matrix.m24 = source.m24;
      matrix.m31 = source.m31;
      matrix.m32 = source.m32;
      matrix.m33 = source.m33;
      matrix.m34 = source.m34;
      matrix.m41 = source.m41;
      matrix.m42 = source.m42;
      matrix.m43 = source.m43;
      matrix.m44 = source.m44;
      return matrix;
   }
   */
/*
class Matrix extends DOMMatrix {

   constructor(input) {
      super(input)
   }
   
   fromMatrix(source) {
      matrix.m11 = source.m11;
      matrix.m12 = source.m12;
      matrix.m13 = source.m13;
      matrix.m14 = source.m14;
      matrix.m21 = source.m21;
      matrix.m22 = source.m22;
      matrix.m23 = source.m23;
      matrix.m24 = source.m24;
      matrix.m31 = source.m31;
      matrix.m32 = source.m32;
      matrix.m33 = source.m33;
      matrix.m34 = source.m34;
      matrix.m41 = source.m41;
      matrix.m42 = source.m42;
      matrix.m43 = source.m43;
      matrix.m44 = source.m44;
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
