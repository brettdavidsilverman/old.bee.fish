openssl req -newkey rsa:2048 -nodes -keyout privatekey.pem -x509 -days 36500 -out cacert.pem -subj "/CN=Feebeecam"