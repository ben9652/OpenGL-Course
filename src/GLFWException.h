#pragma once

#include <exception>

class GLFWException : public std::exception
{
private:
    const char* m_Msg;

public:
    GLFWException(const char* msg) : m_Msg(msg) {}

    virtual const char* what() const throw()
    {
        return m_Msg;
    }
};