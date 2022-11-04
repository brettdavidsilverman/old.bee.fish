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

    class Commands : public std::queue<command_t> {
    protected:
        std::map<command_t, bool> _commands;
        std::mutex _guard;

    public:
        Commands() {

        }

        virtual void push(command_t command) {
            std::lock_guard<std::mutex> lock(_guard);
            if (_commands[command] == false) {
                std::queue<command_t>::push(command);
                _commands[command] = true;
            }
        }

        virtual command_t pop() {
            std::lock_guard<std::mutex> lock(_guard);
            command_t command = front();
            std::queue<command_t>::pop();
            _commands[command] = false;
            return command;
        }

        static void loop(void* param);

    };

    extern Commands commands;


}
    