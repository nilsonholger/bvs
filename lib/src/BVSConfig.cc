#include "BVSConfig.h"

#include<iostream>
#include<fstream>



BVSConfig::BVSConfig(std::string name, int argc, char** argv)
    : name(name)
    , optionStore()
{
    if (argc!=0 && argv!=nullptr)
    {
        loadCommandLine(argc, argv);
    }
}



BVSConfig& BVSConfig::getName(std::string& name)
{
    name = name;
    return *this;
}



BVSConfig& BVSConfig::showOptionStore()
{
    std::cout << "[BVSConfig] OPTION = VALUE" << std::endl;
    for ( auto it : optionStore)
    {
        std::cout << "[BVSConfig] " << it.first << " = " << it.second << std::endl;
    }

    return *this;
}



std::map<std::string, std::string> BVSConfig::dumpOptionStore()
{
    std::map<std::string, std::string> dump = optionStore;

    return dump;
}



BVSConfig& BVSConfig::loadCommandLine(int argc, char** argv)
{
    /* algorithm:
     *  FOR EACH command line argument
     *  DO
     *      CASE --bvs.config=...
     *          CHECK missing argument
     *          SAVE config path
     *      CASE --bvs.options=...
     *          CHECK missing argument
     *          SEPARATE option list into option=value pairs and add
     *  DONE
     */

    // search for --bvs.* command line options
    std::string option;
    std::string configFile;
    for (int i=1; i<argc; i++)
    {
        option = argv[i];

        // check for config
        if (!option.compare(0, 13, "--bvs.config="))
        {
            option.erase(0, 13);

            // check for missing argument
            if (option.empty())
            {
                std::cerr << "[ERROR|BVSConfig] no argument after --bvs.config=" << std::endl;
                exit(1);
            }

            // save config file for later use
            configFile = option;
            //std::cout << "[BVSConfig] --bvs.config: " << configFile << std::endl;
        }

        // check for additional options
        if (!option.compare(0, 14, "--bvs.options="))
        {
            option.erase(0,14);

            // check for missing argument
            if (option.empty())
            {
                std::cerr<< "[ERROR|BVSConfig] no argument after --bvs.options=" << std::endl;
                exit(1);
            }
            //std::cout << "[BVSConfig] --bvs.options: " << option << std::endl;

            // separate option string and add to optionStore
            std::string bvsOption;
            size_t separatorPos = 0;
            size_t equalPos;
            while (separatorPos != std::string::npos)
            {
                separatorPos = option.find_first_of(':');
                bvsOption = option.substr(0, separatorPos);
                equalPos = bvsOption.find_first_of("=");
                optionStore[bvsOption.substr(0, equalPos)] = bvsOption.substr(equalPos+1, bvsOption.size());
                //std::cout << "[BVSConfig] adding: " << bvsOption.substr(0, equalPos) << " -> " << bvsOption.substr(equalPos+1, bvsOption.size()) << std::endl;
                option.erase(0, separatorPos+1);
            }
        }
    }

    if (!configFile.empty()) loadConfigFile(configFile);

    return *this;
}



BVSConfig& BVSConfig::loadConfigFile(const std::string& configFile)
{
    std::ifstream file(configFile.c_str(), std::ifstream::in);
    std::string line;
    std::string tmp;
    std::string option;
    std::string section;
    bool insideQuotes;
    bool append;
    size_t pos;
    size_t posComment;
    int lineNumber = 0;

    // check if file can be read from
    if (!file.is_open())
    {
        std::cerr << "[ERROR|BVSConfig] file not found: " << configFile << std::endl;
        exit(1);
    }

    /* algorithm:
     * FOR EACH line in config file
     * DO
     *      REMOVE all whitespace/tabs, except inside '' and "" pairs
     *          CHECK for ' and "
     *          CHECK for inside quotation
     *          CHECK for whitespace/tabs
     *      IGNORE comments and empty lines
     *      CHECK for section [...]
     *      CHECK section status
     *      CHECK for +option (appending)
     *      FIND delimiter
     *      APPEND option if set
     *          CHECK if option exists
     *      ADD option if not already in store
     *  DONE
     */

    // parse file
    while(getline(file, line))
    {
        tmp.clear();
        insideQuotes = false;
        append = false;
        lineNumber++;

        //remove all whitespace/tabs except inside of '' or "" (useful for whitespace in for example status messages or other output)
        for (unsigned int i=0; i<line.length(); i++)
        {
            // check quotation status, ignore character
            if (line[i]=='\'' || line [i]=='"')
            {
                insideQuotes = !insideQuotes;
                continue;
            }

            // add everything inside quotations
            if (insideQuotes)
            {
                tmp += line[i];
                continue;
            }

            // add only if not whitespace/tabs
            if (line[i]!=' ' && line[i]!='\t') tmp += line[i];
        }

        // ignore comments and empty lines
        if (tmp[0]=='#' || tmp.length()==0) continue;

        // check for section
        if (tmp[0]=='[')
        {
            pos = tmp.find_first_of(']');
            section = tmp.substr(1, pos-1);
            continue;
        }

        // ignore option if section empty
        if (section.empty())
        {
            std::cerr << "[ERROR|BVSConfig] found option belonging to no section in " << configFile << ":" << lineNumber << ": " << line << std::endl;
            exit(1);
        }

        // check for +option (appending)
        if (tmp[0]=='+')
        {
            append = true;
            tmp.erase(0, 1);
        }

        // find '=' delimiter and prepend section name
        pos = tmp.find_first_of('=');
        option = section + '.' + tmp.substr(0, pos);

        // check for empty option name
        if (option.length()==section.length()+1 )
        {
            std::cerr << "[ERROR|BVSConfig] found line starting with '=' in " << configFile << ":" << lineNumber << ": " << line << std::endl;
            exit(1);
        }

        // strip inline comment
        posComment = tmp.find_first_of('#');
        tmp = tmp.substr(pos+1, posComment-pos-1);

        // append option if set
        if (append)
        {
            // check if option exists
            if (optionStore.find(option)==optionStore.end())
            {
                std::cerr << "[ERROR|BVSConfig] cannot append to non existing option in " << configFile << ":" << lineNumber << ": " << line << std::endl;
                exit(1);
            }

            optionStore[option] = optionStore[option] + "," + tmp;
            continue;
        }

        // only add when not already in store, thus command-line options can override config file options and first occurence is used
        if (optionStore.find(option)==optionStore.end())
        {
            optionStore[option] = tmp;
            //std::cout << "[BVSConfig] adding: " << option << " -> " << tmp.substr(pos+1, posComment-pos-1) << std::endl;
        }
    }
    return *this;
}



std::string BVSConfig::searchOption(const std::string& option)
{
    // search for option in store
    if(optionStore.find(option)!=optionStore.end())
    {
        //std::cout << "found: " << option << " --> " << optionStore[option] << std::endl;
        return optionStore[option];
    }
    else
    {
        std::cerr << "[ERROR|BVSConfig] option not found: " << option << std::endl;
        exit(-1);
    }
}



template<> BVSConfig& BVSConfig::convertStringTo<std::string>(const std::string& input, std::string& output)
{
    output = input;
    return *this;
}



template<> BVSConfig& BVSConfig::convertStringTo<bool>(const std::string& input, bool& b)
{
    // check for possible matches to various versions meaning true
    if (input=="1" || input=="true" || input=="True" || input=="TRUE" || input=="on" || input=="On" || input=="ON" || input=="yes" || input=="Yes" || input=="YES")
    {
        b = true;
    }
    else
    {
        b = false;
    }

    return *this;
}

