#include <BVSConfig.h>

#include<fstream>



std::map<std::string, std::string> BVSConfig::optionStore;



BVSConfig::BVSConfig(std::string name)
    : logger("BVS::Config")
{
    (void) name;
}



BVSConfig& BVSConfig::addCommandLine(int argc, char** argv)
{
    // search for --bvs* command line options
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
            LOG(3, "--bvs.config: " << configFile);
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

            LOG(3, "--bvs.options: " << option);

            // separate option string and add to optionStore
            std::string bvsOption;
            size_t separatorPos = 0;
            size_t equalPos;
            while (separatorPos != std::string::npos)
            {
                separatorPos = option.find_first_of(',');
                bvsOption = option.substr(0, separatorPos);
                equalPos = bvsOption.find_first_of(":=");
                optionStore[bvsOption.substr(0, equalPos)] = bvsOption.substr(equalPos+1, bvsOption.size());
                LOG(3, "adding: " << bvsOption.substr(0, equalPos) << " -> " << bvsOption.substr(equalPos+1, bvsOption.size()));
                option.erase(0, separatorPos+1);
            }
        }
    }

    if (!configFile.empty()) addConfigFile(configFile);

    return *this;
}



BVSConfig& BVSConfig::addConfigFile(const std::string& configFile)
{
    std::ifstream file(configFile.c_str(), std::ifstream::in);
    std::string line;
    std::string tmp;
    size_t pos;

    // check if file can be read from
    if (!file.is_open())
    {
        LOG(0, "File not found: " << configFile);
        exit(1);
    }

    // parse file
    while(getline(file, line))
    {
        tmp.clear();
        //remove all whitespace/tabs
        for (unsigned int i=0; i<line.length(); i++)
        {
            if (line[i]!=' ' && line[i]!='\t') tmp+=line[i];
        }
        line = tmp;

        // check for comments and find "=" delimiter
        if (line[0]!='#')
        {
            pos = line.find_first_of('=');
            tmp = line.substr(0, pos);
            if (tmp.length() !=0 )
            {
                // only add when not already in store, thus command-line options can override config file options
                if (optionStore.find(tmp)==optionStore.end())
                {
                    optionStore[tmp] = line.substr(pos+1, line.length());
                }
                LOG(3, "Adding: " << line.substr(0, pos) << " -> " << line.substr(pos+1, line.length()));
            }
            else
            {
                LOG(1, "found line starting with '='");
            }
        }
    }
    return *this;
}



std::string BVSConfig::searchOption(const std::string& option)
{

    std::string shortOpt;
    std::string longOpt;

    // check option string for "s,long", "long,s" or false string
    size_t pos = option.find_first_of(",");
    size_t size = option.size();
    if (pos==1) {
        shortOpt = option.substr(0, pos);
        longOpt = option.substr(pos+1, size);
    } else if (pos==size-2) {
        shortOpt = option.substr(pos+1, size);
        longOpt = option.substr(0, pos);
    } else {
        LOG(3, "no valid option string: " << option);
        return "";
    }

    // check for short options from command line, then for long options
    if(optionStore.find(shortOpt)!=optionStore.end()) {
        LOG(3, "found short option: " << shortOpt << " --> " << optionStore[shortOpt]);
        return optionStore[shortOpt];
    } else if(optionStore.find(longOpt)!=optionStore.end()) {
        LOG(3, "found long option: " << longOpt << " --> " << optionStore[longOpt]);
        return optionStore[longOpt];
    } else {
        LOG(3, "found no option: " << shortOpt << " or " << longOpt);
        return "";
    }
}
