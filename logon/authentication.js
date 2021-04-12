const url = "https://bee.fish";

class Authentication
{
   #secret = null;
   name = null;
   #thumbnail = null;
   #authenticated = false;

   static get thumbnailWidth() 
   {
      return 100;
   }
   
   static get thumbnailHeight()
   {
      return 100;
   }
   
   async logon()
   {
   
      var _this = this;
      
      this.#authenticated = false;
      
      if ( this.name == null ||
           this.name == "" )
         throw "Missing name.";

      if ( !this.hasSecret )
         throw "Missing secret";
         
      var params = {}
      params.method = "POST";
      //params.credentials = "include";
      var body =
         {
            method: "logon",
            name: this.name,
            secret: this.#secret
         }
         
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json());
      
      this.#authenticated =
         data.authenticated;
         
      if ( this.#authenticated )
      {
         if ( this.#thumbnail &&
              !data.thumbnail )
            await this.setThumbnail();
            
         if (data.thumbnail)
           this.#thumbnail = data.thumbnail;
      }
      
      return data.authenticated;
   }
   
   async logout()
   {
      this.#authenticated = false;
      this.#secret = null;
      this.#thumbnail = null;
      this.name = null;
   }
   
   async setThumbnail()
   {
   
      var _this = this;
      
      if ( this.name == null ||
           this.name == "" )
         throw "Missing name";

      if ( !this.hasSecret )
         throw "Missing secret";
         
     if ( !this.#thumbnail )
         throw "Missing thumbnail";

      var params = {}
      params.method = "POST";

      params.body = JSON.stringify(
         {
            method: "setThumbnail",
            name: this.name,
            secret: this.#secret,
            thumbnail: this.#thumbnail
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
   }
   
   async getThumbnail()
   {
   
      var _this = this;
      
      if ( this.name == null ||
           this.name == "" )
         throw "Missing name";

      if ( !this.hasSecret )
         throw "Missing secret";

      var params = {}
      params.method = "POST";

      params.body = JSON.stringify(
         {
            method: "getThumbnail",
            name: this.name,
            secret: this.#secret
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
      this.#thumbnail = data.thumbnail;
   }
   
   get authenticated()
   {
      return this.#authenticated;
   }
   
   get thumbnail()
   {
      return this.#thumbnail;
   }
   
   get hasSecret()
   {
      return (
         this.#secret != null &&
         this.#secret.length > 0
      );
      
   }
   
   async createSecret(secretFile, canvas = null)
   {
      if (this.onprogress)
         this.onprogress(0);
      
      this.#secret = null;
      
      const sha =
         new jsSHA(
            "SHA3-512",
            "B64"
         );

      const pageSize = 100000;
   
      for ( var i = 0;
            i < secretFile.size;
            i += pageSize )
      {
         var percent =
            Math.round((i / secretFile.size) * 100);
         
         var end;
         if (i + pageSize <= secretFile.size)
            end = i + pageSize;
         else
            end = secretFile.size;
      
         var blob = secretFile.slice(i, end);
      
         var buffer = await blob.arrayBuffer();
         var array = new Uint8Array(buffer);
         var base64 = Base64.fromUint8Array(array);
      
         sha.update(base64);
      
         if (this.onprogress)
            this.onprogress(percent);

      }
   
      this.#secret = sha.getHash("B64");
      
      
      if (this.onprogress)
         this.onprogress(100);
         
      if (canvas)
         this.#createThumbnail(secretFile, canvas);


   }
   
   #createThumbnail(secretFile, canvas)
   {
      // Create a thumbail copy from
      // secret file and draw it
      // on the canvas.
      
      var _this = this;
     
      prepareCanvas(canvas);
      
      var image;
      
      
      // Read the secretFile
      var secretFileReader = new FileReader();
      
      // onload fires after reading is complete
      secretFileReader.onload = createImage;
      
      // begin reading
      secretFileReader.readAsDataURL(secretFile);
      
    
      function createImage()
      {
         image = new Image();
         
         image.width =
            Authentication.thumbnailWidth;
            
         image.height =
            Authentication.thumbnailHeight;
            
         image.onload = imageLoaded;
         image.src = secretFileReader.result;
         
      }
      
      function imageLoaded()
      {
   
         var context = canvas.getContext("2d");
         
         // draw the image
         context.drawImage(
            image, 0, 0, canvas.width, canvas.height
         );         

         var jpeg =
            canvas.toDataURL("image/jpeg");
     
         _this.#thumbnail = jpeg;
         
         if (_this.onthumbnailloaded)
            _this.onthumbnailloaded();
      }
      
      function prepareCanvas(canvas)
      {
         canvas.width =
            Authentication.thumbnailWidth;    
         canvas.height =
            Authentication.thumbnailHeight;
         
         var context = canvas.getContext("2d");
         
         // draw the image
         context.clearRect(
            0, 0, canvas.width, canvas.height
         );         
      }
      

   }
   
}
