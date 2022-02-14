#include <Arduino.h>
#include <WiFi.h>
#include <map>

class WebServer : public WiFiServer {


public:

  class WebClient : public WiFiClient {
  private:
    const long KEEP_ALIVE_MILLISECONDS = 5000;

  private:
    bool _currentLineIsBlank = true;
    WebServer* _server;
    unsigned long _startTime = millis();
  public:

    WebClient(WebServer* server, int fd) :
      WiFiClient(fd), 
      _server(server) 
    {
      Serial.print("WebClient::WebClient(");
      Serial.print(fd);
      Serial.println(")");
    }

    virtual ~WebClient() {
      Serial.println("WebClient::~WebClient");
    }

    virtual void loop() {


      // an http request ends with a blank line

      while (available()) {

        char c = read();

        // Reset start time since weve recieved a character
        _startTime = millis();

        Serial.write(c);

        // if you've gotten to the end of the line (received a newline

        // character) and the line is blank, the http request has ended,

        // so you can send a reply

        if (c == '\n' && _currentLineIsBlank) {

          // send a standard http response header

          println("HTTP/1.1 200 OK");

          println("Content-Type: text/html");

          println("Connection: keep-alive");  // the connection will be closed after completion of the response

          println();

          println("<!DOCTYPE HTML>");

          println("<html>");

          // output the value of each analog input pin

          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {

            int sensorReading = analogRead(analogChannel);

            print("analog input ");

            print(analogChannel);

            print(" is ");

            print(sensorReading);

            println("<br />");

          }

          println("</html>");

          break;

        }

        if (c == '\n') {

          // you're starting a new line

          _currentLineIsBlank = true;

        } else if (c != '\r') {

          // you've gotten a character on the current line

          _currentLineIsBlank = false;

        }

      }

      if ((millis() - _startTime) > KEEP_ALIVE_MILLISECONDS)
        stop();

    }

    virtual void stop() {
      
      int socket = fd();
      uint64_t pointer = (uint64_t) _server;

      Serial.print("WebClient::stop(");
      Serial.print(pointer);
      Serial.print(", ");
      Serial.print(socket);
      Serial.println(")");

      _server->_clients.erase(socket);

      WiFiClient::stop();

      Serial.print("WebClient::~stop()");
    }

  };

  std::map<int, WebClient> _clients;
  
  WebClient& getWebClient(WiFiClient& source) {
    int socket = source.fd();
    if (_clients.count(socket) == 0) {
      _clients.emplace(socket, WebClient(this, source.fd()));
    }

    return _clients.at(socket);
  }

  WebServer(int port) : WiFiServer(port) {

  }

  void loop() {

    WiFiClient wiFiClient = available();


    if (wiFiClient) {
      
      WebClient& webClient = getWebClient(wiFiClient);

    }

    for (auto pair : _clients) {
      WebClient& webClient = pair.second;
      webClient.loop();
    }

  }

};
