async function hashFile(file, updateProgress)
{
   var callUpdateProgress =
      (updateProgress instanceof Function);

   const sha =
      new jsSHA(
         "SHA3-512",
         "B64"
      );

   const pageSize = 100000;
   
   for ( var i = 0;
         i < file.size;
         i += pageSize )
   {
      var percent =
         Math.round((i / file.size) * 100);
         
      var end;
      if (i + pageSize <= file.size)
         end = i + pageSize;
      else
         end = file.size;
      
      var blob = file.slice(i, end);
      
      var buffer = await blob.arrayBuffer();
      var array = new Uint8Array(buffer);
      var string = uint8ArrayToString(array);
      var base64 = btoa(string);
      sha.update(base64);
      
      if (callUpdateProgress)
         updateProgress(percent);

   }
   
   const hash = sha.getHash("B64");

   if (callUpdateProgress)
      updateProgress(100);
   
   return hash;
   

   function uint8ArrayToString(array)
   {
      var string =
         String
         .fromCharCode
         .apply(null, array);
      
      return string;
   }
}
