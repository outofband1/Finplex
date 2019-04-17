#pragma once

#include <iostream>
#include <string>

#define PITCH(exceptionType, ...) throw exceptionType(__FILE__, __LINE__, __VA_ARGS__)

namespace Exceptions
{
    class Basic : std::exception
    {
    public:
        Basic(char* fileName, long lineNumber)
        {
            this->fileName = fileName;
            this->lineNumber = lineNumber;
        }

        virtual ~Basic() {}

        const std::string& getFileName() const
        {
            return fileName;
        }

        const long& getLineNumber() const
        {
            return lineNumber;
        }

        virtual void print(std::ostream& stream) const
        {
            stream << "Error occurred in source: " << getFileName() << " at line " << getLineNumber() << std::endl;
        }

    private:
        std::string fileName;

        long lineNumber;
    };

    class OneMessage : public Basic
    {
    public:
        OneMessage(char* fileName, long lineNumber, const std::string& msg) : Basic(fileName, lineNumber)
        {
            this->msg = msg;
        }

        virtual ~OneMessage() {}

        const std::string& getMessage() const
        {
            return msg;
        }

        virtual void print(std::ostream& stream) const
        {
            Basic::print(stream);
            stream << "Message was: " << msg << std::endl;
        }

    private:
        std::string msg;
    };
}
