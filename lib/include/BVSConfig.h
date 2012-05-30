#ifndef BVSCONFIG_H
#define BVSCONFIG_H

#include<map>
#include<memory>
#include<mutex>
#include<sstream>
#include<string>
#include<vector>



/** The BVS configuration mechanism.
 * This is the BVS configuration system. Option-value pairs are taken from the
 * command line and loaded from config files. Option names are handled case
 * insensitive, so 'option' and 'Option' are treated as the same option.
 * The same goes for section names ('[section]').
 *
 * Command Line provided options always override those found in config files,
 * whereas options found in config files are added/updated on a first come,
 * first served basis, so only the first occurence of an option is used.
 *
 * WARNING: This means any redefinition of any option is silently dropped.
 *
 * If a config file is given, it is loaded into the system.
 * The syntax is:
 * @code
 * # lines starting with a '#' sign are considered comments as well as 
 * # everything after a '#' sign
 *
 * option = ignored # options MUST belong to a section
 *
 * [section] # same as [SeCtIoN]
 * option1 = value1 # comment
 * option2 = "value2 with spaces" # single quotes are also possible
 * # options are treated as case insensitive, so option and Option are the same
 *
 * # spaces are stripped if not inside quotes, so these are all valid
 * option3=value
 * option4  =  value
 *  option5     =value
 *
 * booleanOption = true
 * # for boolean type option-value pairs:
 * # '1', 'true', 'True', 'TRUE', 'on', 'On', 'ON', 'yes', 'Yes' or 'YES'
 * # are interpreted as TRUE, everything else as FALSE
 *
 * stringList = elementOne,"element Two",'element Three'
 * booleanList = true, false, True
 * # accessed by using getValue with std::vector<TYPE>, and spaces are
 * # allowed inside single or double quotes (otherwise they will be stripped)
 *
 * # the plus sign can be used to expand existing options, which turns them
 * # into a list, thus allowing for faster reordering
 * list = one
 * +list = two
 * +list = three
 * #+list = four # commented out
 * +list = five
 * list = six # WARNING: redefiniton, will be ignored, see above
 * # the result of this will be: list = one,two,three,five
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
         * @param[in] argc Number of arguments.
         * @param[in] argv Array of arguments.
         */
        BVSConfig(std::string name, int argc = 0, char** argv = nullptr);

        /** Gets the system name.
         * @param[out] name Return object containing name.
         * @return Reference to object.
         */
        BVSConfig& getName(std::string& name);

        /** Loads the given config file (if it exists).
         * This checks the supplied path for a config file and if found parses
         * its contents.
         * @param[in] configFile Name of or path to config file.
         */
        BVSConfig& loadConfigFile(const std::string& configFile);

        /** Prints all variables known by config system to std::cerr.
         * The option names will all be lower case, as this is the way
         * they are represented internally.
         * @return Reference to object.
         */
        BVSConfig& showOptionStore();

        /** Dump the optionStore to a local copy.
         * The option names will all be lower case, as this is the way
         * they are represented internally.
         * @return Dump of optionStore.
         */
        std::map<std::string, std::string> dumpOptionStore();

        /** Template to retrieve value from config by passing it to an argument.
         * Sets default value, if desired option is not found in option store.
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] t This argument will be used to return the retrieved value.
         * @param[in] defaultValue Default value to be returned if desired option is not found.
         * @tparam T Type argument.
         * @return Reference to object.
         */
        template<typename T> BVSConfig& getValue(const std::string& sectionOption, T& t, T defaultValue);

        /** Template to retrieve value from config by return value.
         * Returns default value, if desired option is not found in option store.
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[in] defaultValue Default value to be returned if desired option is not found.
         * @tparam T Type argument.
         * @return The desired option's value.
         */
        template<typename T> T getValue(const std::string& sectionOption, T defaultValue);

        /** Template to retrieve a value list from config as a std::vector of chosen type.
         * @tparam T Type used for value list.
         * @param[in] sectionOption The desired option, should be in form "section.option".
         * @param[out] t This vectore will be used to return the retrieved value list.
         * @return Reference to object.
         */
        template<typename T> BVSConfig& getValue(const std::string& sectionOption, std::vector<T>& t);

    private:
        std::string name; /**< Instance's name. */
        std::mutex mutex; /**< Mutex for thread safety. */

        /** A map of all stored options. */
        std::map<std::string, std::string, std::less<std::string>> optionStore;

        /** Loads the given arguments into the system.
         * This checks argv for occurences of --bvs.config and --bvs.options.
         * If found, they are added to the internal option-value storage.
         * \param[in] argc Size of argv.
         * @param[in] argv Array of arguments.
         * @return Reference to object.
         */
        BVSConfig& loadCommandLine(int argc, char** argv);

        /** Searches optionStore for the given option name.
         * @param[in] option Desired config option.
         * @return The option's value if found.
         */
        std::string searchOption(std::string option);

        /** Convert std::string to desired type.
         * @param[in] input Input string.
         * @param[out] t Converted argument of desired type.
         * @return Reference to Object.
         */
        template<typename T> BVSConfig& convertStringTo(const std::string& input, T& t);

        /** Convert std::string to desired type.
         * @param[in] input Input string.
         * @return Converted argument of desired type.
         */
        template<typename T> T convertStringTo(const std::string& input);

        friend class BVS;
};



template<typename T> BVSConfig& BVSConfig::getValue(const std::string& sectionOption, T& t, T defaultValue)
{
    t = getValue(sectionOption, defaultValue);
    return *this;
}



template<typename T> T BVSConfig::getValue(const std::string& sectionOption, T defaultValue)
{
    (void) defaultValue;
    std::string tmp = searchOption(sectionOption);
    if (tmp.length()!=0)
    {
        return convertStringTo<T>(tmp);
    }
    else
    {
        return defaultValue;
    }
}



template<typename T> BVSConfig& BVSConfig::getValue(const std::string& sectionOption, std::vector<T>& t)
{
    // get list from optionStore and if empty, abort
    std::string tmp = searchOption(sectionOption);
    if (tmp.length()==0)
    {
        return *this;
    }

    // separate list into substrings on occurence of ',', push to vector
    size_t separatorPos = 0;
    while (separatorPos != std::string::npos)
    {
        separatorPos = tmp.find_first_of(',');
        t = t;
        t.push_back(convertStringTo<T>(tmp.substr(0, separatorPos)));
        tmp.erase(0, separatorPos+1);
    }

    return *this;
}



template<typename T> BVSConfig& BVSConfig::convertStringTo(const std::string& input, T& t)
{
    std::istringstream stream(input);
    stream >> t;
    return *this;
}



template<typename T> T BVSConfig::convertStringTo(const std::string& input)
{
    T t;
    convertStringTo(input, t);
    return t;
}



template<> BVSConfig& BVSConfig::convertStringTo<std::string>(const std::string& input, std::string& output);
template<> BVSConfig& BVSConfig::convertStringTo<bool>(const std::string& input, bool& b);



#endif //BVSCONFIG_H

