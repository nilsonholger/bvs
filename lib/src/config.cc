#include <algorithm>
#include <iostream>
#include <fstream>

#include "bvs/config.h"



BVS::Config::Config(const std::string& name, int argc, char** argv, std::string file)
	: name(name)
	, mutex()
	, optionStore()
{
	if (argc!=0 && argv!=nullptr) loadCommandLine(argc, argv);

	if (file!=std::string()) loadConfigFile(file);
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
	 *      CASE --$name.config=...
	 *          CHECK missing argument
	 *          SAVE config path
	 *      CASE --$name.options=...
	 *          CHECK missing argument
	 *          SEPARATE option list into option=value pairs and add
	 *  DONE
	 */

	// search for --$name.* command line options
	std::string arg;
	std::string configFile;
	for (int i=1; i<argc; i++)
	{
		arg = argv[i];

		// check for config
		if (!arg.compare(0, 10 + name.length(), "--" + name + ".config="))
		{
			arg.erase(0, 13);

			// check for missing argument
			if (arg.empty())
			{
				std::cerr << "[ERROR|Config] no argument after --" + name + ".config=" << std::endl;
				exit(1);
			}

			// save config file for later use
			configFile = arg;
		}

		// check for additional options
		if (!arg.compare(0, 11 + name.length(), "--" + name + ".options="))
		{
			arg.erase(0,14);

			// check for missing argument
			if (arg.empty())
			{
				std::cerr << "[ERROR|Config] no argument after --" + name + ".options=" << std::endl;
				exit(1);
			}

			// separate arg string and add to optionStore
			std::string option;
			size_t separatorPos = 0;
			size_t equalPos;
			std::lock_guard<std::mutex> lock(mutex);
			while (separatorPos != std::string::npos)
			{
				// separate
				separatorPos = arg.find_first_of(':');
				option = arg.substr(0, separatorPos);

				// optionName to lowercase
				equalPos = option.find_first_of("=");
				std::transform(option.begin(), option.begin()+equalPos, option.begin(), ::tolower);

				// add
				optionStore[option.substr(0, equalPos)] = option.substr(equalPos+1, option.size());
				arg.erase(0, separatorPos+1);
			}
		}
	}

	// load config file if given on command line
	if (!configFile.empty()) loadConfigFile(configFile);

	return *this;
}



BVS::Config& BVS::Config::loadConfigFile(const std::string& configFile)
{
	std::ifstream file(configFile.c_str(), std::ifstream::in);

	// check if file can be read from
	if (!file.is_open())
	{
		std::cerr << "[ERROR|Config] file not found: " << configFile << std::endl;
		exit(1);
	}

	std::string line, tmp, option, section;
	bool insideQuotes, append;
	size_t pos;
	int lineNumber = 0;

	std::lock_guard<std::mutex> lock(mutex);

	/* algorithm:
	 * FOR EACH line in config file
	 * DO
	 *      IGNORE comments and empty lines
	 *      REMOVE all unquoted whitespace/tabs, strip inline comments
	 *          CHECK for ' and "
	 *          COPY with/without whitespace/tabs
	 *      CHECK section existence
	 *      FIND delimiter, separate
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

		//remove all unquoted whitespace/tabs, strip inline comments
		for (unsigned int i=0; i<line.length(); i++)
		{
			if (line[i]=='#') break;
			if (line[i]=='\'' || line [i]=='"')
			{
				insideQuotes = !insideQuotes;
				continue;
			}
			if (insideQuotes) tmp += line[i];
			else if (!isspace(line[i])) tmp += line[i];
		}

		// ignore empty lines and comments
		if (line.length()==0) continue;
		if (line[0]=='#') continue;

		// REMOVE at some future point in time
		if (line[0]=='+')
		{
			std::cerr << "[ERROR|Config] '+option' DEPRECATED, please use the new '+=' operator instead (sorry for the syntax change)" << std::endl;
			exit(1);
		}

		// check section existence
		if (tmp[0]=='[')
		{
			pos = tmp.find_first_of(']');
			section = tmp.substr(1, pos-1);
			continue;
		}
		if (section.empty())
		{
			std::cerr << "[ERROR|Config] option without section." << std::endl;
			std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << std::endl;
			exit(1);
		}

		// find '='/'+=' delimiter, prepend section name, separate
		pos = tmp.find_first_of("=");
		if (tmp.at(pos-1)=='+')
		{
			append = true;
			tmp.erase(pos-1, 1);
			pos--;
		}
		option = section + '.' + tmp.substr(0, pos);
		tmp.erase(0, pos+1);

		// check for empty option name
		if (option.length()==section.length()+1 )
		{
			std::cerr << "[ERROR|Config] starting line with '='." << std::endl;
			std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << std::endl;
			exit(1);
		}

		// to lowercase
		std::transform(option.begin(), option.end(), option.begin(), ::tolower);

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
	else
	{
		// search if section exists
		bool found = false;
		bool foundSection = false;
		size_t separatorPos = option.find_first_of('.');
		std::string section = option.substr(0, separatorPos);
		std::string match;
		std::vector<std::string> sections;
		for (auto& it: optionStore)
		{
			separatorPos = it.first.find_first_of('.');
			match = it.first.substr(0, separatorPos);
			if (section==match) found = true;
			for (auto& it2: sections)
				if (it2==match) foundSection = true;
			if (!foundSection) sections.push_back(match);
		}
		if (!found)
		{
			std::cerr << "[WARNING|Config] SECTION NOT FOUND: [" << section << "]" << std::endl;
			std::cerr << "[WARNING|Config] follwing sections were encountered: ";
			for (auto& it: sections)
				std::cerr << "[" << it << "] ";
			std::cerr << std::endl;
		}
	}

	return std::string();
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

