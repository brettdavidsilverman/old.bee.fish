const url = "https://bee.fish";

class Authentication
{
   secret = null;
   _authenticated = false;
   
   async logon()
   {
   
      var _this = this;
      
      this._authenticated = false;
      
      if ( !this.hasSecret )
         throw new Error("Missing secret");
         
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "logon",
            secret: this.secret
         }
         
      params.body = JSON.stringify(body);

      var data = await
         fetch(url, params)
         .then(response => response.json())
         .catch(error => {throw new Error(error)});

      this._authenticated =
         data.authenticated;
      
      return this.authenticated;
   }
   
   async getStatus()
   {
   
      var _this = this;
      
      this._authenticated = false;
      
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
         .then(response => response.json())
         .catch(error => {throw new Error(error)});

      
      this._authenticated =
         data.authenticated;
         
      alert(JSON.stringify(data));
      
      return this.authenticated;
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
         .then(response => response.json())
         .catch(error => {throw new Error(error)});

         
      this._authenticated = false;
      this.secret = null;
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