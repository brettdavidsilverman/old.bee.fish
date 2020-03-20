function encodeNumberArray(array) {

   var bytes = array.length * 64 / 8;
   var arrayBuffer = new ArrayBuffer(bytes);
   var dataView = new DataView(arrayBuffer);

   for (var i = 0; i < array.length; i++) {
      dataView.setFloat64(i * 64 / 8, array[i]);
   }

   var base64 = arrayBufferToBase64(arrayBuffer);
   return base64;
}

function arrayBufferToBase64( buffer ) 
{
   var binary = '';
   var bytes = new Uint8Array( buffer );
   var len = bytes.byteLength;
   for (var i = 0; i < len; i++) {
      binary += String.fromCharCode( bytes[ i ] );
   }
   return window.btoa( binary );
}


function decodeNumberArray(base64) {

   arrayBuffer = base64ToArrayBuffer(base64);

   dataView = new DataView(arrayBuffer);

   var floats = arrayBuffer.byteLength * 8 / 64;

   var array = new Array(floats);

   for (var i = 0; i < floats; i++) {
      array[i] = dataView.getFloat64(i * 64 / 8);
   }
   
   return array;
}


function base64ToArrayBuffer(base64) {
   var binary_string = window.atob(base64);
   var len = binary_string.length;
   var bytes = new Uint8Array( len ); 
   for (var i = 0; i < len; i++) {
      bytes[i] = binary_string.charCodeAt(i);
   }
   return bytes.buffer;
}

