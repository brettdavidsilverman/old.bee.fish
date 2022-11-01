#pragma once
#include <queue>
#include <map>

#define RESTART_AFTER_ERROR() FeebeeCam::restartAfterError(__FILE__, __FUNCTION__, __LINE__)

namespace FeebeeCam {

    bool initializeCommands();
    bool putToSleep();

    void restartAfterError(const char* file, const char* function, int line);
    bool onConnectedToInternet();
    bool onCommand(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    void commandLoop(void*);

    enum command_t {
        DO_NOTHING,
        INITIALIZE_WEBSERVER,
        INTERNET,
        SAVE_SETUP,
 //       READ_WEATHER,
        UPLOAD_WEATHER,
        DOWNLOAD_FILES,
        PUT_TO_SLEEP,
        RESTART,
        STOP_CAMERA
    };

    extern std::mutex guard;

    class Commands : public std::queue<command_t> {
    protected:
        std::map<command_t, bool> _commands;

    public:
        Commands() {

        }

        virtual void push(command_t command) {
            if (_commands[command] == false) {
                std::queue<command_t>::push(command);
                _commands[command] = true;
            }
        }

        virtual command_t pop() {
            command_t command = front();
            std::queue<command_t>::pop();
            _commands[command] = false;
            return command;
        }
    };

    extern Commands commands;


}
