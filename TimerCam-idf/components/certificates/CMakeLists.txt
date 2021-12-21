idf_component_register(SRCS "certificates.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "certs/cacert.pem"
                                   "certs/privatekey.pem")
