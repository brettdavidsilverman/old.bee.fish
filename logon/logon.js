var url = "https://bee.fish";

async function logon(email, password)
{
   
   var params = {}
   params.method = "POST";
   params.credentials = "include";
   params.body = JSON.stringify(
      {
         method: "logon",
         email: email,
         password: password
      }
   );
   var status;
   
   fetch(url, params)
   .then(response => response.json())
   .then(data => console.log(data))
   .catch(function(error) {
      console.log( "Request failed " + error);
   });
}