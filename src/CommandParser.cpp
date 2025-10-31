#include "CommandParser.h"
#include <string.h>

CommandParser::CommandParser()
    : _commandCount(0){}

void CommandParser::add_command(const char *command, void (*callback)(ScanUtil* scan), bool echo)
{
    if (_commandCount < MAX_COMMANDS)
    {
        strncpy(_command[_commandCount].name, command, MAX_COMMAND_LENGTH - 1);
        _command[_commandCount].name[MAX_COMMAND_LENGTH - 1] = '\0';
        _command[_commandCount].callback = callback;
        _command[_commandCount].echo = echo;
        _commandCount++;
    }
    else
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "Command limit reached (%d)", MAX_COMMANDS);
        if (_errorCallback)
            _errorCallback(error_message);
        if (_debug_stream)
            _debug_stream->println(error_message);
    }
}

void CommandParser::processInput(char c, bool verbose)
{
    if(!_inputBuffer)
        _inputBuffer = new CaptureBuffer(128, '\n'); // Example size and terminator

    if(_inputBuffer->add(c))
    {
        if(_debug_stream)
        {
            _debug_stream->println("Terminator found, processing input:");
            _inputBuffer->dumpTo(*_debug_stream);
            _debug_stream->println();
        }
        processInput(_inputBuffer->buffer(), _inputBuffer->size());
        _inputBuffer->clear();
    }
    if (verbose)
    {
        _inputBuffer->dumpTo(Serial);
        Serial.println();
    }
}

void CommandParser::processInput(char *buffer, uint8_t buffer_size)
{
    ScanUtil scan(buffer, buffer_size);
    scan.normalize(); // Normalize the input string to UPPERCASE and remove accents

    if (scan.seek(_prefix)) // "CMD,3165,"
    {
        char cmd_buf[MAX_COMMAND_LENGTH];
        cmd_buf[0] = '\0'; // Initialize command buffer
        save_last_command_alnum(scan);
        if(_debug_stream)
        {
            _debug_stream->print("CommandParser: \'");
            _debug_stream->write(scan.current(), scan.remaining());
            _debug_stream->println("\'");
        }
        if(scan.contains(delimiter))
        {
            scan.substring(cmd_buf, delimiter, MAX_COMMAND_LENGTH);
        }
        else
        {
            scan.substring(cmd_buf, '\0', MAX_COMMAND_LENGTH);
        }
        handleCommand(cmd_buf, MAX_COMMAND_LENGTH, &scan);
    }
    else if (_errorCallback)
        _errorCallback("Prefix not found");
}

void CommandParser::handleCommand(char* command, uint8_t cmdlen, ScanUtil* scan)
{
    for (uint8_t i = 0; i < _commandCount; i++)
    {
        if (strncmp(command, _command[i].name, cmdlen) == 0)
        {
            if (_command[i].echo)
                memcpy(_previous_command_backup, _last_command_alnum, MAX_COMMAND_LENGTH);
            else
                memcpy(_last_command_alnum, _previous_command_backup, MAX_COMMAND_LENGTH);
            if (_command[i].callback)
                _command[i].callback(scan);
            return;
        }
    }
    memcpy(_last_command_alnum, _previous_command_backup, MAX_COMMAND_LENGTH);
    if (_errorCallback)
    {
        char error_message[128];
        snprintf(error_message, sizeof(error_message), "Command \'%s' not found", command);
        _errorCallback(error_message);
    }
}
