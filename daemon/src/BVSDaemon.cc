#include "BVS.h"
#include<unistd.h>



BVS* bvs;
BVSLogger logger("BVSDaemon");



int setup(int argc, char** arcv);
int testLogger();
int testConfig();



int main(int argc, char** argv)
{
	setup(argc, argv);

	LOG(2, "loading Modules!");
	bvs->loadModules();
//
	LOG(2, "starting!");
	bvs->start();

	std::string input;
	while (input != "q" && input != "quit")
	{
		std::getline(std::cin, input);

		if (input == "r" || input == "run")
		{
			LOG(2, "RUN!!!");
			bvs->run();
		}
		else if (input.empty() || input == "s" || input == "step")
		{
			LOG(2, "STEP!!!");
			bvs->step();
		}
		else if (input == "p" || input == "pause")
		{
			LOG(2, "PAUSING!!!");
			bvs->pause();
		}
		else if (input == "t" || input == "test")
		{
			testLogger();
			testConfig();
		}
		else if (input == "q" || input == "quit")
		{
			LOG(2, "quitting...");
			bvs->quit();
			LOG(2, "finished!");
		}
		else if (input == "h" || input == "help")
		{
			std::cout << "usage:" << std::endl;
			std::cout << "   r|run     run system until paused" << std::endl;
			std::cout << "   s|step    advance system by one step" << std::endl;
			std::cout << "   p|pause   pause(stop) system" << std::endl;
			std::cout << "   t|test    call test functions" << std::endl;
			std::cout << "   q|quit    shutdown system and quit" << std::endl;
			std::cout << "   h|help    help" << std::endl;
		}
	}

	return 0;
}



int setup(int argc, char** argv)
{
	bvs = new BVS(argc, argv);
	//bvs.enableLogFile("BVSLog.txt");
	//bvs.enableLogConsole();

	LOG(2, "starting!");

	return 0;
}



int testLogger()
{
	LOG(0, "to CLI FILE");
	bvs->disableLogConsole();
	LOG(0, "to FILE only");
	BVSLogger file("FILE LOG", 3, BVSLogger::TO_FILE);
	file.out(0) << "FILE ONLY" << std::endl;
	file.endl();
	bvs->enableLogConsole();
	BVSLogger cli("CLI LOG", 3, BVSLogger::TO_CLI);
	cli.out(0) << "CLI ONLY" << std::endl;
	cli.endl();
	bvs->disableLogConsole();
	bvs->disableLogFile();
	LOG(0, "NOOP");
	bvs->enableLogConsole();
	LOG(0, "to CLI");
	bvs->disableLogConsole();
	bvs->enableLogFile("BVSLog.txt", true);
	LOG(0, "to FILE AGAIN");
	bvs->enableLogConsole();
	BVSLogger both("to BOTH", 0, BVSLogger::TO_CLI_AND_FILE);
	both.out(0) << "to CLI AND FILE" << std::endl;
	both.endl();

	return 0;
}



int testConfig()
{
	LOG(0, "dump all config options to std::cout!");
	bvs->config.showOptionStore();
	LOG(0, "testing...");

	int i;
	std::string s;
	bool b;
	bvs->config.getValue("BVS.logVerbosity", i, 0)
		.getValue("BVS.logFile", s, std::string("default"))
		.getValue("BVS.logSystem", b, false);

	LOG(0, "Getting int: " << i);
	LOG(0, "Getting string: " << s);
	LOG(0, "Getting bool: " << b);

	i = bvs->config.getValue<int>("BVS.logVerbosity", 0);
	s = bvs->config.getValue<std::string>("BVS.logFile", std::string("default"));
	b = bvs->config.getValue<bool>("BVS.logSystem", false);
	LOG(0, "Getting int directly: " << i);
	LOG(0, "Getting string directly: " << s);
	LOG(0, "Getting bool directly: " << b);

	std::string empty;
	bvs->config.getValue("this.option.does.not.exist", empty, std::string("empty"));
	LOG(0, "This should be 'empty': " << empty);

	std::vector<std::string> list;
	bvs->config.getValue("BVS.modules", list);
	LOG(0, "List: BVS.modules");
	int count = 0;
	for (auto it : list)
	{
		LOG(0, count++ << ": " << it);
	}

	return 0;
}
