#include <algorithm>
#include <iostream>
#include <fstream>

#include "bvs/config.h"

using BVS::Config;



Config::Config(const std::string& name, const int argc, const char** argv)
	: name{name},
	mutex{},
	optionStore{},
	sections{},
	fileStack{}
{
	if (argc!=0 && argv!=nullptr) loadCommandLine(argc, argv);
}



std::map<std::string, std::string> Config::dumpOptionStore() const
{
	std::lock_guard<std::recursive_mutex> lock(mutex);

	std::map<std::string, std::string> dump = optionStore;

	return dump;
}



Config& Config::loadCommandLine(const int argc, const char** argv)
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
				error("Command Line", 0, "--"+name+".config", "NO ARGUMENT GIVEN.");

			// save config file for later use
			configFile = arg;
		}

		// check for additional options
		if (!arg.compare(0, 11 + name.length(), "--" + name + ".options="))
		{
			arg.erase(0,14);

			// check for missing argument
			if (arg.empty())
				error("Command Line", 0, "--"+name+".options", "NO ARGUMENTS GIVEN.");

			// separate arg string and add to optionStore
			std::string option;
			size_t separatorPos = 0;
			size_t equalPos;
			std::lock_guard<std::recursive_mutex> lock{mutex};
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



Config& Config::loadConfigFile(const std::string& configFile)
{
	std::ifstream file{configFile.c_str(), std::ifstream::in};

	// check if file can be read from
	if (!file.is_open())
	{
		if (fileStack.empty())
			error(name, 0, configFile, "FILE NOT FOUND.");
		else
			error(fileStack.top().first, fileStack.top().second, "source " + configFile, "FILE NOT FOUND.");
	}

	std::string line, tmp, option, section;
	bool insideQuotes, append;
	size_t pos;
	int lineNumber = 0;

	std::lock_guard<std::recursive_mutex> lock{mutex};

	/* algorithm:
	 * FOR EACH line in config file
	 * DO
	 *      IGNORE comments and empty lines
	 *      REMOVE all unquoted whitespace/tabs, strip inline comments
	 *          CHECK for ' and "
	 *          COPY with/without whitespace/tabs
	 *      CHECK 'source' command
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
		for (size_t i=0; i<line.length(); i++)
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
		if (tmp.length()==0) continue;
		if (tmp[0]=='#') continue;

		// check 'source' command
		if (tmp.substr(0,6)=="source" && tmp[6]!='=')
		{
			std::string source = tmp.substr(6, tmp.length()-6);
			fileStack.push(std::pair<std::string, int>(configFile, lineNumber));
			loadConfigFile(source);
			fileStack.pop();
			continue;
		}

		// check section existence
		if (tmp[0]=='[')
		{
			pos = tmp.find_first_of(']');
			section = tmp.substr(1, pos-1);
			if (pos==std::string::npos)
				error(configFile, lineNumber, line, "MISSING ']' IN SECTION DECLARATION.");

			std::transform(section.begin(), section.end(), section.begin(), ::tolower);
			if (sections.find(section)==sections.end()) sections[section] = configFile + ":" + std::to_string(lineNumber);
			else error(configFile, lineNumber, line, "DUPLICATE, FIRST SEEN HERE: " + sections.find(section)->second);
			continue;
		}
		if (section.empty())
			error(configFile, lineNumber, line, "OPTION WITHOUT SECTION.");

		// find '='/'+=' delimiter, prepend section name, separate
		pos = tmp.find_first_of("=");
		if (pos==std::string::npos)
			error(configFile, lineNumber, line, "NO SEPARATOR ('=') FOUND.");
		if (pos>0 && tmp.at(pos-1)=='+')
		{
			append = true;
			tmp.erase(pos-1, 1);
			pos--;
		}
		option = section + '.' + tmp.substr(0, pos);
		tmp.erase(0, pos+1);

		// check for empty option name
		if (option.length()==section.length()+1 )
			error(configFile, lineNumber, line, "STARTING LINE WITH '='.");

		// to lowercase
		std::transform(option.begin(), option.end(), option.begin(), ::tolower);

		// append option if set
		if (append)
		{
			// check if option exists
			if (optionStore.find(option)==optionStore.end())
				error(configFile, lineNumber, line, "CANNOT APPEND TO A NEW OPTION.");

			optionStore[option] = optionStore[option] + "," + tmp;
			continue;
		}

		// only add when not already in store, thus command-line options overrides config file
		if (optionStore.find(option)==optionStore.end()) optionStore[option] = tmp;
	}

	return *this;
}



std::string Config::searchOption(std::string option) const
{
	std::transform(option.begin(), option.end(), option.begin(), ::tolower);

	std::lock_guard<std::recursive_mutex> lock{mutex};

	// search for option in store
	if(optionStore.find(option)!=optionStore.end())
	{
		std::string tmp = optionStore.find(option)->second;
		return tmp;
	}
	else
	{
		// search if section exists
		size_t separatorPos = option.find_first_of('.');
		std::string section = option.substr(0, separatorPos);
		if (sections.find(section)==sections.end())
		{
			std::cerr << "[WARNING|Config] SECTION NOT FOUND: [" << section << "]" << std::endl;
			std::cerr << "[WARNING|Config] follwing sections were encountered: ";
			for (auto& s: sections)
				std::cerr << "[" << s.first << "]:" << s.second << "  ";
			std::cerr << std::endl;
		}
	}

	return notFound;
}



inline void Config::error(const std::string& configFile, int lineNumber, const std::string& line, const std::string& message) const
{
	std::cerr << "[ERROR|Config] " << configFile << ":" << lineNumber << ": " << line << " <=== " << message <<  std::endl;
	exit(1);
}



#ifndef __ANDROID_API__
template<> const Config& Config::convertStringTo<std::string>(const std::string& input, std::string& output) const
{
	output = input;

	return *this;
}
#endif



template<> const Config& Config::convertStringTo<bool>(const std::string& input, bool& b) const
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

