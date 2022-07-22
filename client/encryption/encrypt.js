async function example() {

    const encode = (e => e.encode.bind(e))(new TextEncoder);

    let { publicKey: pub1, privateKey: key1 } = await crypto.subtle.generateKey({ name: 'ECDH', namedCurve: 'P-521' }, true, ['deriveKey']); // generate key pairs

    // get server ecdh public key
    let { publicKey: pub2, privateKey: key2 } = await crypto.subtle.generateKey({ name: 'ECDH', namedCurve: 'P-521' }, true, ['deriveKey']); // generate key pairs
//    let jwk = await fetch('/others public key').then(res=>res.json())
//    let spub = await crypto.subtle.importKey('jwk', jwk., { name: 'ECDH', namedCurve: 'P-521' }, false, [])

    // use pub2 and key1 derive a ase key
    let pub2key1 = await crypto.subtle.deriveKey({ name: 'ECDH', namedCurve: 'P-521', public: pub2 }, key1, { name: 'AES-GCM', length: 256 }, true, ["encrypt", "decrypt"]);

    // now use gcm to encrypt or decrypt
    //let text = crypto.subtle.encrypt({ name: 'AES', length: 256 }, pub2key1 , encode('hello world'));
    let iv = crypto.getRandomValues(new Uint8Array(12));

    let cipherText = await crypto.subtle.encrypt(
        {
            name: "AES-GCM",
            iv: iv
        }, 
        pub2key1,
        encode('hello world')
    );
        
    // same on the server
    var decoder = new TextDecoder("utf-8");
    var text = decoder.decode(encryptedArrayBuffer);
    let decryptedArrayBuffer = await crypto.subtle.decrypt(
        {
            name: "AES-GCM",
            iv: iv
        }, 
        key1,
        cipherText
    );
    alert(decryptedArrayBuffer);
    /*
    text.then(
        arrayBuffer => decoder.decode(arrayBuffer)
    ).then(
        text => alert(text)
    ).catch(
        error => alert(error)
    );
    */
}

example();
