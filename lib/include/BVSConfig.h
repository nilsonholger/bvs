#ifndef BVSCONFIG_H
#define BVSCONFIG_H

#include<map>
#include<memory>
#include<sstream>
#include<string>

#include "BVSLogger.h"


// TODO comments for everything in BVSConfig
class BVSConfig
{
    public:
        BVSConfig(std::string name);

        std::string getName();

        BVSConfig& loadCommandLine(int argc, char** argv);
        BVSConfig& loadConfigFile(const std::string& configFile);

        template<typename T> BVSConfig& getValue(std::string sectionOption, T& t);
        template<typename T> T getValue(std::string sectionOption);

        BVSConfig& getValue(std::string sectionOption, bool& value);
        BVSConfig& getValue(std::string sectionOption, std::string& value);

        // TODO add getValue for std::vector

    private:
        std::string name;
        BVSLogger logger;
        std::map<std::string, std::string, std::less<std::string>> optionStore;

        std::string searchOption(const std::string& option);
};


template<typename T> BVSConfig& BVSConfig::getValue(std::string sectionOption, T& t)
{
    std::istringstream stream(searchOption(sectionOption));
    stream >> t;
    return *this;
}



template<typename T> T BVSConfig::getValue(std::string sectionOption)
{
    T t;
    getValue(sectionOption, t);
    return t;
}

#endif //BVSCONFIG_H

