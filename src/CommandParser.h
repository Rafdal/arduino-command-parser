#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include "ScanUtil.h"
#include "CaptureBuffer.hpp"

#define MAX_COMMANDS        16
#define MAX_COMMAND_LENGTH  32
#define MAX_PREFIX_LENGTH   16

class CommandParser {
public:
    CommandParser();

    void add_command(const char* command, void (*callback)(ScanUtil* scan), bool echo = true);
    
    void processInput(char* buffer, uint8_t buffer_size);
    void processInput(char c, bool verbose = false);
    
    inline void set_prefix(const char* prefix)
    {
        strncpy(_prefix, prefix, MAX_PREFIX_LENGTH - 1);
        _prefix[MAX_PREFIX_LENGTH - 1] = '\0'; // Ensure null termination
        _prefixLength = strlen(_prefix);
    }

    inline void on_error(void (*callback)(char* error_message)) { _errorCallback = callback; }

    inline char* last_command(void) { return _last_command_alnum; }

    inline void set_debug_stream(Stream* stream) { _debug_stream = stream; }

    char delimiter = ','; // Default delimiter

private:
    struct Command {
        char name[MAX_COMMAND_LENGTH];
        void (*callback)(ScanUtil* scan);
        bool echo;
    };

    CaptureBuffer* _inputBuffer = nullptr; // Unused if processing full strings

    char _last_command_alnum[MAX_COMMAND_LENGTH] = {0}; // Last command received
    char _previous_command_backup[MAX_COMMAND_LENGTH] = {0}; // Backup of the last command

    Stream* _debug_stream = nullptr; // Optional debug stream

    Command _command[MAX_COMMANDS];
    uint8_t _commandCount;

    char _prefix[MAX_PREFIX_LENGTH] = {0}; // Command prefix
    uint8_t _prefixLength;

    void (*_errorCallback)(char* error_message) = nullptr;
    void handleCommand(char* command, uint8_t cmdlen, ScanUtil* scan);

    void save_last_command_alnum(ScanUtil& scan)
    {
        unsigned int pos = scan.pos;
        const char* exclude = ",";
        char replace = ' ';
        if (!scan.extract(_last_command_alnum, exclude, MAX_COMMAND_LENGTH, replace))
        {
            strcpy(_last_command_alnum, "SCANUTIL::EXTRACT ERR");
        }
        scan.pos = pos; // Restore original position
    }
};

#endif // COMMAND_PARSER_H