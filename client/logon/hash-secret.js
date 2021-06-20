importScripts("sha256.js");

onmessage = async function(e) {
   const sha = new jsSHA("SHA-256", "ARRAYBUFFER");
      
   var file = e.data;
   
   var bytes = await file.arrayBuffer();
   
   sha.update(bytes);
      
   const hash = sha.getHash("B64");
   
   postMessage(hash);
}