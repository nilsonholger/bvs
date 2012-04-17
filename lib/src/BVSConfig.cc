#include "BVSConfig.h"

#include<fstream>



BVSConfig::BVSConfig(std::string name)
    : name(name)
    , logger("BVS::Config")
    , optionStore()
{

}



std::string BVSConfig::getName()
{
    return name;
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
                LOG(0, "no argument after --bvs.config=");
                exit(1);
            }

            // save config file for later use
            configFile = option;
            LOG(2, "--bvs.config: " << configFile);
        }

        // check for additional options
        if (!option.compare(0, 14, "--bvs.options="))
        {
            option.erase(0,14);

            // check for missing argument
            if (option.empty())
            {
                LOG(0, "no argument after --bvs.options=");
                exit(1);
            }

            LOG(2, "--bvs.options: " << option);

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
                LOG(3, "adding: " << bvsOption.substr(0, equalPos) << " -> " << bvsOption.substr(equalPos+1, bvsOption.size()));
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
    size_t pos;

    // check if file can be read from
    if (!file.is_open())
    {
        LOG(0, "file not found: " << configFile);
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
     *      FIND delimiter
     *      ADD option if not already in store
     *  DONE
     */

    // parse file
    while(getline(file, line))
    {
        tmp.clear();
        insideQuotes = false;

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
        line = tmp;

        // ignore comments and empty lines
        if (line[0]=='#' || line.length()==0) continue;

        // check for section
        if (line[0]=='[')
        {
            pos = line.find_first_of(']');
            section = line.substr(1, pos-1);
            continue;
        }

        // ignore option if section empty
        if (section.empty())
        {
            LOG(1, "found option belonging to no section, ignoring it!");
            continue;
        }

        // find '=' delimiter and prepend section name
        pos = line.find_first_of('=');
        option = section + '.' + line.substr(0, pos);

        // check for empty option name
        if (option.length()==section.length()+1 )
        {
            LOG(1, "found line starting with '=', ignoring it!");
            continue;
        }

        // only add when not already in store, thus command-line options can override config file options and first occurence is used
        if (optionStore.find(option)==optionStore.end())
        {
            optionStore[option] = line.substr(pos+1, line.length());
            LOG(3, "adding: " << option << " -> " << line.substr(pos+1, line.length()));
        }
    }
    return *this;
}



BVSConfig& BVSConfig::getValue(std::string sectionOption, bool& value)
{
    // get stored value
    std::string tmp = searchOption(sectionOption);
    LOG(0, tmp);

    // check for possible matches to various versions meaning true
    if (tmp=="1" || tmp=="true" || tmp=="True" || tmp=="TRUE" || tmp=="on" || tmp=="On" || tmp=="ON" || tmp=="yes" || tmp=="Yes" || tmp=="YES")
    {
        value = true;
    }
    else
    {
        value = false;
    }
    return *this;
}



BVSConfig& BVSConfig::getValue(std::string sectionOption, std::string& value)
{
    value = searchOption(sectionOption);
    return *this;
}



std::string BVSConfig::searchOption(const std::string& option)
{

    if(optionStore.find(option)!=optionStore.end())
    {
        LOG(3, "found: " << option << " --> " << optionStore[option]);
        return optionStore[option];
    }
    else
    {
        LOG(1, "not found: " << option);
        return "";
    }
}

