const url = "https://bee.fish";

class Authentication
{
   secret = null;
   name = null;
   localThumbnail = null;
   _serverThumbnail = null;
   _authenticated = false;

   async logon()
   {
   
      var _this = this;
      
      this._authenticated = false;
      
      if ( this.name == null ||
           this.name == "" )
         throw "Missing name.";

      if ( !this.hasSecret )
         throw "Missing secret";
         
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
         
      if ( this._authenticated )
      {
         if ( this.localThumbnail &&
              !data.thumbnail )
         {
        
            await this._setThumbnail();
         }
         
      }
      
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
         
      this._authenticated = false;
      this.secret = null;
      this.localThumbnail = null;
      this.name = null;
   }
   
   async _setThumbnail()
   {
   
      var _this = this;
      
      if ( this.name == null ||
           this.name == "" )
         throw "Missing name";

      if ( !this.hasSecret )
         throw "Missing secret";
         
     if ( !this.localThumbnail )
         throw "Missing thumbnail";

      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "setThumbnail",
            name: this.name,
            secret: this.secret,
            thumbnail: this.localThumbnail
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
      params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "getThumbnail",
            name: this.name,
            secret: this.secret
         }
      );

      var data = await
         fetch(url, params)
         .then(response => response.json());
         
      this._serverThumbnail = data.thumbnail;
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
