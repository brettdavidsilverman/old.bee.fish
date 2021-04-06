const url = "https://bee.fish";

class Authentication
{
   #secret = null;
   username = null;
   #thumbnail = null;
   #authenticated = false;
   #usernameExists = false;
   
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
      
      if ( this.username == null ||
           this.username == "" )
         throw "Missing username.";

      var params = {}
      params.method = "POST";
      //params.credentials = "include";
      var body =
         {
            method: "logon",
            username: this.username,
            secret: this.#secret
         }
         
      if (!await this.checkUsernameExists())
      {
         body.thumbnail = this.thumbnail;
      }
      
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json());
      
      this.#authenticated =
         data.authenticated;
         
      return data.authenticated;
   }
   
   async logout()
   {
      this.#authenticated = false;
      this.#secret = null;
      this.#thumbnail = null;
      this.username = null;
      this.#usernameExists = false;
   }
   
   async checkUsernameExists()
   {
   
      var _this = this;
      
      if ( this.username == null ||
           this.username == "" )
         throw "Missing username.";

      var params = {}
      params.method = "POST";

      params.body = JSON.stringify(
         {
            method: "checkUsernameExists",
            username: this.username
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
      this.#usernameExists = data.usernameExists;
      
      return this.usernameExists;
   }
   
   async getThumbnail()
   {
   
      var _this = this;
      
      if ( this.username == null ||
           this.username == "" )
         throw "Missing username.";

      var params = {}
      params.method = "POST";

      params.body = JSON.stringify(
         {
            method: "getThumbnail",
            username: this.username
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
   
   get usernameExists()
   {
      return this.#usernameExists;
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
   
   async hashFile(file, canvas = null, progress = null)
   {
      if (this.onprogress)
         this.onprogress(0);
      
      if (canvas)
         this.#createThumbnail(file, canvas);
         
      await this.#createFileHash(file, progress);
   }
   
   async #createFileHash(file, progress)
   {

      this.#secret = null;
      
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
      
         if (this.onprogress)
            this.onprogress(percent);

      }
   
      this.#secret = sha.getHash("B64");
      
      
      if (this.onprogress)
         this.onprogress(100);
   }
   
   #createThumbnail(file, canvas)
   {
      var _this = this;
     
      prepareCanvas(canvas);
      
      var image;
      
      
      // Read the file
      var fileReader = new FileReader();
      
      // onload fires after reading is complete
      fileReader.onload = createImage;
      
      // begin reading
      fileReader.readAsDataURL(file);
      
    
      function createImage()
      {
         image = new Image();
         
         image.width =
            Authentication.thumbnailWidth;
            
         image.height =
            Authentication.thumbnailHeight;
            
         image.onload = imageLoaded;
         image.src = fileReader.result;
         
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
