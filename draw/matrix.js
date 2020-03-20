class Matrix extends Array {
   constructor(i, j, ...values) {
      super(...values);
      this.i = i;
      this.j = j;
      if (values.length == 0)
         this.length = i * j;
   }
   
   multiply(rhs, result) {
   
      if (result == undefined)
         result =
            new Matrix(rhs.i, this.j);
         
      for ( var i = 0;
            i < rhs.i;
            i++ )
      {
         for ( var j = 0;
               j < this.j;
               j++ )
         {
            var sum = 0;
            
            for ( var k = 0;
                  k < this.j;
                  k++ )
            {
               var lhsVal =
                  this[
                     k + j * this.i
                  ];
                  
               var rhsVal =
                  rhs[
                     i + k * rhs.i
                  ];
                  
              sum += lhsVal * rhsVal;
            }
            
            result[
               i + j * result.i
            ] = sum;
         }
      }
      
      return result;
   }
   
   toString(precision) {
   
      if (precision == undefined)
         precision = 0;
      
      var rows = new Array(this.j);
 
      for ( var row = 0;
            row < this.j;
            row++ )
      {
         // get the row
         rows[row] = this.slice(
            this.i * row,
            this.i * (row + 1)
         ).map(round).join(",\t");

      }

      var string = rows.join("\n");
         
      return string;
      
      function round(number) {
         return Math.round(
            number *
            10 ** precision
         ) / 10 ** precision;
      }
   }
   
}

Matrix.Identity = class extends Matrix {
   constructor(length) {
      super(length, length)
      
      for ( var j = 0;
            j < length;
            j++ )
      {
         for ( var i = 0;
               i < length;
               i++ )
         {
            this[i + j * length] = 
               (i == j) ? 1 : 0;
         }
      }
   }
   
}

class Matrix2d extends Matrix {
   constructor(...values) {
      super(3, 3, ...values);
   }
   
   scale(scale) {
      return this.multiply(
         new Matrix2d(
            scale.x, 0, 0,
            0, scale.y, 0,
            0, 0, 1
         ),
         new Matrix2d()
      );
   }
   
   translate(point) {
      return this.multiply(
         new Matrix2d(
            1, 0, point.x,
            0, 1, point.y,
            0, 0, 1
         ),
         new Matrix2d()
      );
   }
   
   applyToContext(context) {
   
      // a, b, e,  0, 1, 2,
      // c, d, f,  3, 4, 5
      // 0, 0, 1   6, 7, 8
      
      context.setTransform(
         this[0], // a
         this[1], // b
         this[3], // c
         this[4], // d
         this[2], // e
         this[5]  // f
      );
   }
   
   transformPoint(point) {
      
      var transformedPoint =
         super.multiply(
            point,
            new Point()
         );
         
      return transformedPoint;
   }
   
   multiply(rhs) {
      return super.multiply(
         rhs,
         new Matrix2d()
      );
   }
   
   inverse() {
   
      var a = this;
      var out = new Matrix2d();
      
      var a00 = a[0],
          a01 = a[1],
          a02 = a[2];
    
      var a10 = a[3],
          a11 = a[4],
          a12 = a[5];
          
      var a20 = a[6],
          a21 = a[7],
          a22 = a[8];
          
      var b01 = a22 * a11 - a12 * a21;
      var b11 = -a22 * a10 + a12 * a20;
      var b21 = a21 * a10 - a11 * a20;
      
      // Calculate the determinant

      var det = a00 * b01 + 
                a01 * b11 +
                a02 * b21;

      if (!det) {
         return null;
      }

      det = 1.0 / det;
      out[0] = b01 * det;
      out[1] = (-a22 * a01 + a02 * a21)
               * det;
      out[2] = (a12 * a01 - a02 * a11) 
               * det;
      out[3] = b11 * det;
      out[4] = (a22 * a00 - a02 * a20) 
               * det;
      out[5] = (-a12 * a00 + a02 * a10)
               * det;
      out[6] = b21 * det;
      
      out[7] = (-a21 * a00 + a01 * a20)
               * det;
      out[8] = (a11 * a00 - a01 * a10)
               * det;
    
      return out;
      
   }

}

Matrix2d.Identity =
   class extends Matrix2d
{
   constructor() {
      super( 1, 0, 0,
             0, 1, 0,
             0, 0, 1 );
   }
}

class Point2d extends Matrix {
   constructor(...values) {
      super(1, 3, ...values);
   }
   
   get x() {
      return this[0];
   }
   
   set x(value) {
      this[0] = value;
   }
   
   get y() {
      return this[1];
   }
   
   set y(value) {
      this[1] = value;
   }
   
   negative() {
      return new Point(
         -this.x, 
         -this.y,
         1
      );
   }
   
   reciprocal() {
      return new Point(
         1 / this.x,
         1 / this.y,
         1
      );
   }
   
   toString() {
   
      return JSON.stringify(
         {
            x: this.x,
            y: this.y
         }
      );
      
   }
   
   toJSON() {
   
      return {
         x: this.x,
         y: this.y
      };
      
   }
   
}

const Point = Point2d;

