function logout() {
/*
   const headers = new Headers({
      "Authorization": "Digest xxx"
   });
   
   fetch('https://bee.fish/index.html', {
      credentials: 'include',
      username: "abc",
      password: "123",
      headers: headers
   });
   */
	// To invalidate a basic auth login:
	// 
	// 	1. Call this logout function.
	// 	2. It makes a GET request to an URL
	//    with false Basic Auth credentials
	//	 3. The URL returns a 401 Unauthorized
	// 	4. Forward to some "you-are-logged-out"-page
	// 	5. Done, the Basic Auth header is invalid now
   

   jQuery.ajax({
  //    type: "GET",
      url: "https://wrong:0123@bee.fish",
      type: "GET",
      async: "false",
      username: "logmeout",
      password: "123456",
      headers: { "Authorization": "Basic xxx" }
   })
   .done(function(){
      // If we don't get an error,
      // we actually got an error 
      // as we expect an 401!
      throw new Error("Expected 401");
   })
   .fail(function(){
      // We expect to get an
      // 401 Unauthorized error!
      // In this case we are successfully 
      // logged out and we redirect the user.
      window.location = "/index.html";
   });

}

