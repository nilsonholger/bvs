#ifndef BVSCONFIG_H
#define BVSCONFIG_H

#include<map>
#include<sstream>
#include<string>

#include "BVSLogger.h"



class BVSConfig
{
    public:
        BVSConfig(std::string name);
        BVSConfig& addCommandLine(int argc, char** argv);
        BVSConfig& addConfigFile(const std::string& configFile);
        template<class T> BVSConfig& getValue(std::string option, T& t);
        template<class T> T getValue(std::string option);

    private:
        BVSLogger logger;
        std::string searchOption(const std::string& option);
        static std::map<std::string, std::string, std::less<std::string>> optionStore;
};


template<class T> BVSConfig& BVSConfig::getValue(std::string option, T& t)
{
    std::istringstream stream(searchOption(option));
    stream >> t;
    return *this;
}



template<class T> T BVSConfig::getValue(std::string option)
{
    std::istringstream stream(searchOption(option));
    T t;
    stream >> t;
    return t;
}

#endif //BVSCONFIG_H

