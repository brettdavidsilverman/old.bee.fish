openssl req -new \
-newkey rsa:2048 -nodes -keyout feebeecam.local.key \
-out feebeecam.local.crt \
-subj "/C=AU/ST=Gold Coast/L=Berett/O=bee.fish/OU=IT/CN=bee.fish" \
-days 36500