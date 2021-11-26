class Authentication
{
   _authenticated = false;
   
   url = document.location.origin;
   
   constructor(input) {
      Object.assign(this, input);
   }
   
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

      return await
         fetch(this.url, params)
         .then(response => response.json())
         .then(data => _this._authenticated = data.authenticated)
         .catch(error => {throw new Error(error)});
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
      
      var _this = this;

      return await
         fetch(this.url, params)
         .then(response => response.json())
         .then(data => _this._authenticated = data.authenticated)
         .catch(error => {throw new Error(error)});
         
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
         fetch(this.url, params)
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