var authentication = new Authentication();

authentication.getStatus().then(
   function(authenticated)
   {
      if (!authenticated)
      {
         sessionStorage.setItem(
            "authentication.logon-from",
            window.location.href
         );
         
         window.location.assign(
            "/client/logon"
         );
      }
      else
         sessionStorage.setItem(
            "authentication.logon-from",
            null
         );
   }
);