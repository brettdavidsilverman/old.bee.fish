const url = "https://bee.fish";

class Authentication
{
   #fileHash;
   username;
   #authenticated = false;
   
   async logon()
   {
   
      var _this = this;
      
      this.#authenticated = false;
      
      if ( this.username == null ||
           this.username == "" )
         throw "Missing username.";

      if ( this.#fileHash == null )
         throw "Missing file hash.";
         
      var params = {}
      params.method = "POST";
      //params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "logon",
            username: this.username,
            secret: this.#fileHash
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
      
      
      this.#authenticated =
         data.authenticated;
            
      return data.authenticated;
   }
   
   get authenticated()
   {
      return this.#authenticated;
   }
   
   async hashFile(file, progress)
   {

      this.#fileHash = null;
      
      if (progress)
         progress.value = 0;
         
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
         var base64 = Base64.fromUint8Array(array);
      
         sha.update(base64);
      
         if (progress)
            progress.value = percent;

      }
   
      this.#fileHash = sha.getHash("B64");
      
      if (progress)
         progress.value = 100;
      
   }
   
}
