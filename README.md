# bee.fish
Files to create bee.fish website

This contains all the files used to research and design https://bee.fish

The files under /server are used to create an actual https server.

Install these libraries using <b>sudo apt install</b>
	libssl-dev
	libboost-all-dev
	libcrypto++-dev

Use the <b>make</b> command to build the C++ programs.

Then <b>make install</b>

The rest of the files are html or js and provide examples of what I've been researching.
The /client directory has the layers needed to create the /client/draw app (work in progress).

The /server/database directory has files needed to create a bit hierarchical database.
Currently the https://bee.fish/client/storage program uses it as key/value pair database.

The https://bee.fish/Arduino directories contain ESP32 files for the https://bee.fish/beehive/ camera (work in progress)