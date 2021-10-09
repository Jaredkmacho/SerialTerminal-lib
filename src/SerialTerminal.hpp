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


#ifndef SERIAL_TERMINAL_H
#define SERIAL_TERMINAL_H

#include <Arduino.h>
#include <vector>
#include <string>
#include <sstream>

#define COMMAND_MAX_LENGHT 256

class SerialTerminal : public HardwareSerial {
public:
    explicit SerialTerminal(int uart_nr=0, char newLineChar='\n');

    void addCommand(const char *command, void(*function)());
    void setDefaultHandler(void (*function)(std::string command));

    void setSerialEcho(bool doEcho);
    void setPostCommandHandler(void (*function)(void));

    void readSerial();
    void clearBuffer();
    void clearStream();

    std::string getNext();
    std::string getRemaining();

private:
    struct SerialTerminalCallback {
        std::string command;
        void (*function)();
    };

    std::vector<SerialTerminalCallback> commandList_;
    int numCommands_;
    char newlineChar_;
    std::string rxBuffer_;
    std::istringstream commandStream;

    bool doCharEcho;
    void (*postCommandHandler_)(void);
    void (*defaultHandler_)(std::string command);
};




#endif /* SERIAL_TERMINAL_H */