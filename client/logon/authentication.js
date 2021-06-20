const url = "https://bee.fish";

class Authentication
{
   _authenticated = false;
   
   async logon(secret)
   {
   
      var _this = this;
      
      this._authenticated = false;
      
      if ( secret == null || !secret.length )
         throw new Error("Missing secret");
         
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "logon",
            secret: secret
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
         
      console.log(document.cookie);
      
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
   
   
 
   
}