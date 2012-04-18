#ifndef BVSCONFIG_H
#define BVSCONFIG_H

#include<map>
#include<memory>
#include<sstream>
#include<string>
#include<vector>

#include "BVSLogger.h"


/** The BVS configuration mechanism.
 * This is the BVS configuration system. Option-value pairs are taken from the
 * command line and loaded from config files.
 * 
 * Command Line provided options always override those found in config files,
 * whereas options found in config files are added/updated on a first come,
 * first served basis, so only the first occurence of an option is used.
 *
 * If a config file is given, it is loaded into the system.
 * The syntax is:
 * @code
 * # lines starting with a '#' sign are considered comments
 *
 * option = ignored # options MUST belong to a section, others are ignored
 *
 * [section]
 * option1 = value1 # also a valid comment
 * option2 = "value2 with spaces" # single quotes are also possible
 *
 * # spaces are stripped if not inside quotes, so these are all valid
 * option3=value
 * option4  =  value
 *  option5     =value
 *
 * booleanOption = true
 * # for bool type option-value pairs:
 * # '1', 'true', 'True', 'TRUE', 'on', 'On', 'ON', 'yes', 'Yes' or 'YES'
 * # are interpreted as TRUE, everything else as FALSE
 *
 * optionList = elementOne,"element Two",'element Three'
 * # accessed by using getValue with std::vector<std::string>, and spaces are
 * # allowed inside single or double quotes (otherwise they will be stripped)
 * @endcode
 * @see loadConfigFile
 *
 * The function loadCommandLine checks for occurence of
 * --bvs.config=$CONFIG and --bvs.options=$OPTIONS.
 * Command line syntax is similar to config file syntax:
 * @code
 * --bvs.options=option1=value1:optionString="value2 with spaces":optionList=elementOne,"element Two",'element Three'
 * @endcode
 * The separator for bvsOptions is a ':' sign, so ':' are not allowed anywhere
 * else, not even inside single or double quotes. Also, be careful with spaces
 * when not in single or double quotes, as your $SHELL will most likely use
 * them as an argument delimiter (you have been warned)
 * @see loadCommandLine
 */
class BVSConfig
{
    public:
        /** Construct config system.
         * @param[in] name A unique name for this config system.
         */
        BVSConfig(std::string name);

        /** Gets the system name.
         * @return The name.
         */
        std::string getName();

        /** Loads the given arguments into the system.
         * This checks argv for occurences of --bvs.config and --bvs.options.
         * If found, they are added to the internal option-value storage.
         * \param[in] argc Size of argv.
         * @param[in] argv Array of arguments.
         * @return Reference to object.
         */
        BVSConfig& loadCommandLine(int argc, char** argv);

        /** Loads the given config file (if it exists).
         * This checks the supplied path for a config file and if found parses
         * its contents.
         * @param[in] configFile Name of or path to config file.
         */
        BVSConfig& loadConfigFile(const std::string& configFile);

        /** Template to retrieve value from config by passing it to an argument.
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] t This argument will be used to return the retrieved value.
         * @tparam T Type argument.
         * @return Reference to object.
         */
        template<typename T> BVSConfig& getValue(std::string sectionOption, T& t);

        /** Template to retrieve value from config by return value.
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @tparam T Type argument.
         * @return The desired option's value.
         */
        template<typename T> T getValue(std::string sectionOption);

        /** Gets value from config as a boolean (overload for template).
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] value This argument will be used to return the retrieved value.
         * @return Reference to object.
         */
        BVSConfig& getValue(std::string sectionOption, bool& value);

        /** Gets value from config as a string (overload for template).
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] value This argument will be used to return the retrieved value.
         * @return Reference to object.
         */
        BVSConfig& getValue(std::string sectionOption, std::string& value);

        /** Gets value from config as a vector, useful for lists (overload for template).
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] value This argument will be used to return the retrieved value.
         * @return Reference to object.
         */
        BVSConfig& getValue(std::string sectionOption, std::vector<std::string>& value);


    private:
        std::string name; /**< Instance's name. */
        BVSLogger logger; /**< Config's logging instance. */

        /** A map of all stored options. */
        std::map<std::string, std::string, std::less<std::string>> optionStore;

        /** Searches optionStore for the given option name.
         * @param[in] option Desired config option.
         * @return The option's value if found.
         */
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

