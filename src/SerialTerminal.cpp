/*
 * MIT License
 *
 * Copyright (c) 2018-2021 Erriez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * This library is inspired by the work of Erriez Serial Terminal found in: https://github.com/Erriez/ErriezSerialTerminal
 * and is adapted for ESP32 Arduino framework and STL
 * */

#include "SerialTerminal.hpp"

/**
 * @brief SerialTerminal constructor.
 * @param uart_nr UART peripheral number default 0
 * @param newLineChar default \\n
 * */
SerialTerminal::SerialTerminal(int uart_nr, char newLineChar) :
    HardwareSerial(uart_nr),
    numCommands_(0),
    newlineChar_(newLineChar),
    postCommandHandler_(nullptr),
    defaultHandler_(nullptr)
{
    clearBuffer();
}

/**
 * @brief clearBuffer method
 * */
void SerialTerminal::clearBuffer() {
    rxBuffer_ = "";
    rxBuffer_.clear();
}

/**
 * @brief clearStream method
 * */
void SerialTerminal::clearStream() {
    commandStream.str("");
    commandStream.clear();
}


/**
 * @brief Add command with callback handler
 * @param command: Register a null-terminated ASCII command
 * @param function: function to be called on matching command
 * */
void SerialTerminal::addCommand(const char *command, void (*function)()) {
    commandList_.emplace_back(SerialTerminalCallback{std::string(command), function});
    numCommands_++;
}

/**
 * @brief Set the echo configuration
 * @param doEcho true will enable echo to any printable character
 * */
void SerialTerminal::setSerialEcho(bool doEcho)
{
    doCharEcho = doEcho;
}

/**
 * @brief Set post command handler callback, this will be called after any handled command.
 * @param function Address of the callback function.
 * */
void SerialTerminal::setPostCommandHandler(void (*function)()) {
    postCommandHandler_ = function;
}

/**
 * @brief Set default callback handler for unknown commands.
 * @param function Address of the default handler.
 * */
void SerialTerminal::setDefaultHandler(void (*function)(std::string command)){
    defaultHandler_ = function;
}

/**
 * @brief read and process commands when newline character has been received
 * */
void SerialTerminal::readSerial() {
    bool matched = false;
    std::string command;
    char c;

    while (available() > 0) {
        // Read one character form serial port
        c = static_cast<char>(read());

        // Check newline character 
        if ( c == newlineChar_ ) {
            //Echo received char
            if(doCharEcho) println();

            clearStream();

            // Search for matching commands
            commandStream.str(rxBuffer_);
            commandStream >> command;

            if(!command.empty()) {
                for(SerialTerminalCallback stc : commandList_) {
                    if(command == stc.command) {
                        // Call command callback handler
                        (*stc.function)();
                        matched = true;
                        break;
                    }
                }
            
                if(!matched && (defaultHandler_ != nullptr)) {
                    (*defaultHandler_)(command);
                }
            }

            //Run post command handler
            if (postCommandHandler_ != nullptr) {
                (*postCommandHandler_)();
            }

            clearBuffer();

        } else if (c == '\b' || c == 127) { //Either backspace or delete char
            if(!rxBuffer_.empty()) {
                rxBuffer_.pop_back();
                //Echo received char
                if (doCharEcho) print("\b \b");
            }
        } else if (isprint(c)) {
            // Store printable characters
            rxBuffer_ += c;
            if(doCharEcho) print(c);
        }
    }
}

/**
 * @brief Get the next argument
 * @return string containing the next argumnet token
 * */
std::string SerialTerminal::getNext()
{
    std::string token;
    commandStream >> token;
    return token;
}

/**
 * @brief Get the remaining content from the command
 * @return string containing the remaining characters
 * */
std::string SerialTerminal::getRemaining()
{
    char data[COMMAND_MAX_LENGHT];
    commandStream.readsome(data, COMMAND_MAX_LENGHT);
    return std::string(data);
}

