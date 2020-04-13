#ifndef SECURE_CONFIG_H
#define SECURE_CONFIG_H

const char Server::cert_pem[] =
   "/etc/letsencrypt/live/bee.fish/fullchain.pem";
   
const char Server::key_pem[]  =
   "/etc/letsencrypt/live/bee.fish/privkey.pem";
   
const char Server::error_log[] =
   "/home/bee/error.log";

const char Server::request_log[] =
   "/home/bee/bee.fish/request.log";
   
#define DEBUG

#endif