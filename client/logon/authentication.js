const url = "https://bee.fish";

class Authentication
{
   name = null;
   secret = null;
   serverThumbnail = null;
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
         
      this.serverThumbnail = null;
      
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

      return this.authenticated;
   }
   
   async getStatus()
   {
   
      var _this = this;
      
      this._authenticated = false;
      this.serverThumbnail = null;
      
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
         this.serverThumbnail = data.thumbnail;
      else
         this.serverThumbnail = null;
         
      return data.authenticated;
   }
   
   async logoff()
   {
      var _this = this;
      
      this._authenticated = false;

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
      this.name = null;
   }
   
   async setThumbnail()
   {
      if ( !this.serverThumbnail )
         throw new Error("Missing thumbnail");

      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.body = JSON.stringify(
         {
            method: "setThumbnail",
            thumbnail: this.serverThumbnail
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
         
      this.serverThumbnail =
         data.thumbnail;
      
      return this.serverThumbnail;
   }
   
   get authenticated()
   {
      return this._authenticated;
   }
   
   
   get hasSecret()
   {
      return (
         this.secret != null &&
         this.secret.length > 0
      );
      
   }
 
   
}