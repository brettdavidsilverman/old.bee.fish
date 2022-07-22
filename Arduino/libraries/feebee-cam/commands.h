#pragma once
#include <queue>
#include <map>

namespace FeebeeCam {

    bool initializeCommands();
    
    //void commandLoop(void*);
    
    void handleCommands();

    void putToSleep();

    bool initializeSettings();

    enum command_t {
        DO_NOTHING,
        INITIALIZE_WEBSERVER,
        INTERNET,
        SAVE_SETUP,
 //       READ_WEATHER,
        UPLOAD_WEATHER,
        PUT_TO_SLEEP,
        RESTART
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

    void commandLoop(void*);

}
