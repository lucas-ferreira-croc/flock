#ifndef ID_COMPONENT_HPP
#define ID_COMPONENT_HPP

#include <string>
#include "log/logger.hpp"

struct IDComponent
{
    std::string _name;
    IDComponent(const std::string name = "")
    {
        this->_name = name;
        Logger::warning(_name);
    }

};

#endif