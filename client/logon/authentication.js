const url = "https://bee.fish";

class Authentication
{
   name = null;
   secret = null;
   localThumbnail = null;
   _serverThumbnail = null;
   _authenticated = false;

   async logon()
   {
   
     // await this.logoff();
      
      var _this = this;
      
      this._authenticated = false;
      
      if ( this.name == null ||
           this.name == "" )
         throw new Error("Missing name");

      if ( !this.hasSecret )
         throw new Error("Missing secret");
         
      this._serverThumbnail = null;
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "logon",
            name: this.name,
            secret: this.secret
         }
         
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json());
      
      this._authenticated =
         data.authenticated;
      /*
      if ( this._authenticated )
      {
         if ( this.localThumbnail &&
              !data.thumbnail )
         {
        
            await this._setThumbnail();
         }
         
      }
      */
      return this.authenticated;
   }
   
   async getStatus()
   {
   
      var _this = this;
      
      this._authenticated = false;
      this._serverThumbnail = null;
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "getStatus"
         }
         
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json());
      
      this._authenticated =
         data.authenticated;
         
      if ( this._authenticated &&
           data.thumbnail )
         this._serverThumbnail = data.thumbnail;
      else
         this._serverThumbnail = null;
         
      return data.authenticated;
   }
   
   async logoff()
   {
      var _this = this;
      
      this._authenticated = false;
      this.localThumbnail = null;
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "logoff"
         }
         
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
      sessionStorage.clear();
      
      this._authenticated = false;
      this.secret = null;
      this.localThumbnail = null;
      this.name = null;
   }
   
   async setThumbnail()
   {
      if ( !this.localThumbnail )
         throw new Error("Missing thumbnail");

      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "setThumbnail",
            thumbnail: this.localThumbnail
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
   }
   
   async getThumbnail()
   {
   
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "getThumbnail"
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json())
         .catch(error => alert(error));
         
      this._serverThumbnail =
         data.thumbnail;
      
      return this._serverThumbnail;
   }
   
   get authenticated()
   {
      return this._authenticated;
   }
   
   get serverThumbnail()
   {
      return this._serverThumbnail;
   }
   
   get hasSecret()
   {
      return (
         this.secret != null &&
         this.secret.length > 0
      );
      
   }
 
   
}