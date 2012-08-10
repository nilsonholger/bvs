#include "bvs/config.h"

#include<algorithm>
#include<iostream>
#include<fstream>



BVS::Config::Config(std::string name, int argc, char** argv)
	: name(name)
	, mutex()
	, optionStore()
{
	if (argc!=0 && argv!=nullptr) loadCommandLine(argc, argv);
}



BVS::Config& BVS::Config::getName(std::string& name)
{
	name = name;
	return *this;
}



std::map<std::string, std::string> BVS::Config::dumpOptionStore()
{
	std::lock_guard<std::mutex> lock(mutex);

	std::map<std::string, std::string> dump = optionStore;

	return dump;
}



BVS::Config& BVS::Config::loadCommandLine(int argc, char** argv)
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
				std::cerr << "[ERROR|Config] no argument after --bvs.config=" << std::endl;
				exit(1);
			}

			// save config file for later use
			configFile = option;
		}

		// check for additional options
		if (!option.compare(0, 14, "--bvs.options="))
		{
			option.erase(0,14);

			// check for missing argument
			if (option.empty())
			{
				std::cerr << "[ERROR|Config] no argument after --bvs.options=" << std::endl;
				exit(1);
			}

			// separate option string and add to optionStore
			std::string bvsOption;
			size_t separatorPos = 0;
			size_t equalPos;
			std::lock_guard<std::mutex> lock(mutex);
			while (separatorPos != std::string::npos)
			{
				// search for ':' and '='
				separatorPos = option.find_first_of(':');
				bvsOption = option.substr(0, separatorPos);
				equalPos = bvsOption.find_first_of("=");

				// optionName to lowercase
				std::transform(bvsOption.begin(), bvsOption.begin()+equalPos, bvsOption.begin(), ::tolower);

				// add
				optionStore[bvsOption.substr(0, equalPos)] = bvsOption.substr(equalPos+1, bvsOption.size());
				option.erase(0, separatorPos+1);
			}
		}
	}

	// load config file if given on command line
	if (!configFile.empty()) loadConfigFile(configFile);

	return *this;
}



BVS::Config& BVS::Config::loadConfigFile(const std::string& configFile)
{
	/* algorithm:
	 * FOR EACH line in config file
	 * DO
	 *      REMOVE all whitespace/tabs, except inside '' and "" pairs
	 *          CHECK for ' and "
	 *          CHECK for inside quotation
	 *          CHECK for whitespace/tabs
	 *      IGNORE comments and empty lines
	 *      CHECK for section [...]
	 *      CHECK for orphaned options (no section)
	 *      CHECK for +option (appending)
	 *      FIND delimiter
	 *      APPEND option if set
	 *          CHECK if option exists
	 *      ADD option if not already in store
	 *  DONE
	 */

	std::ifstream file(configFile.c_str(), std::ifstream::in);

	// check if file can be read from
	if (!file.is_open())
	{
		std::cerr << "[ERROR|Config] file not found: " << configFile << std::endl;
		exit(1);
	}

	std::string line;
	std::string tmp;
	std::string option;
	std::string section;
	bool insideQuotes;
	bool append;
	size_t pos;
	size_t posComment;
	int lineNumber = 0;

	std::lock_guard<std::mutex> lock(mutex);

	// parse file
	while(getline(file, line))
	{
		tmp.clear();
		insideQuotes = false;
		append = false;
		lineNumber++;

		//remove all whitespace/tabs except inside of '' or ""
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
			if (!isspace(line[i])) tmp += line[i];
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

		// check for orphaned options
		if (section.empty())
		{
			std::cerr << "[ERROR|Config] option without section." << std::endl;
			std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << std::endl;
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

		// to lowercase
		std::transform(option.begin(), option.end(), option.begin(), ::tolower);

		// check for empty option name
		if (option.length()==section.length()+1 )
		{
			std::cerr << "[ERROR|Config] starting line with '='." << std::endl;
			std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << std::endl;
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
				std::cerr << "[ERROR|Config] cannot append to a non existing option." << std::endl;
				std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << std::endl;
				exit(1);
			}

			optionStore[option] = optionStore[option] + "," + tmp;
			continue;
		}

		// only add when not already in store, thus command-line options overrides config file
		if (optionStore.find(option)==optionStore.end()) optionStore[option] = tmp;
	}
	return *this;
}



std::string BVS::Config::searchOption(std::string option) const
{
	std::transform(option.begin(), option.end(), option.begin(), ::tolower);

	std::lock_guard<std::mutex> lock(mutex);

	// search for option in store
	if(optionStore.find(option)!=optionStore.end())
	{
		std::string tmp = optionStore.find(option)->second;
		return tmp;
	}
	else return std::string();
}



template<> const BVS::Config& BVS::Config::convertStringTo<std::string>(const std::string& input, std::string& output) const
{
	output = input;

	return *this;
}



template<> const BVS::Config& BVS::Config::convertStringTo<bool>(const std::string& input, bool& b) const
{
	// check for possible matches to various versions meaning true
	if (input=="1"
			|| input=="true" || input=="True" || input=="TRUE"
			|| input=="on" || input=="On" || input=="ON"
			|| input=="yes" || input=="Yes" || input=="YES")
	{
		b = true;
	}
	else b = false;

	return *this;
}

