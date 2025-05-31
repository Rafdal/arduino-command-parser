# command-parser

```c++
#include <CommandParser.h>
CommandParser commandParser;

void setup() {


    commandParser.add_command("LED", [](ScanUtil* scan) {
        if(scan->equals("ON")) {
            // on

        } else if (scan->equals("OFF")) {
            // off

        } else if (scan->equals("ONOFF")) {
            
            // onoff (toggle)

        } else if (scan->equals("DIMMER")) {
            // dimmer
        } else {
           // Unknown Command
        }
    });

    commandParser.add_command("ADD", [](ScanUtil* scan) {
        uint8_t x,y;
        if (scan->get_uint8_t(&x))
        {
            if(scan->get_uint8_t(&y))
            {
                // do something with x,y
                uint8_t z = x+y;
                return;
            }
        }
        // Parse error
    });


    commandParser.on_error([](char* error_message) {
        // print error msg
    });

    // more code
}

void loop() {

    char text[64];
    unsigned int size = your_function(text); // implement your own logic to get the text
    
    if (size > 0) {
        commandParser.processInput(text, size); // parse and execute commands
    }
}

```