#include "bvs/bvs.h"



BVS::BVS* bvs;
BVS::Logger logger("Daemon");



int testLogger();
int testConfig();



/** BVSD namespace, contains only the bvs daemon. */
namespace BVSD
{
/** BVS framework's command line daemon.
 * This is a simple command line daemon. It serves as a client to the BVS
 * framework. It is also intended as a sample client.
 *
 * It is interactive, you can use the following commands by just entering them
 * on the command line and then pressing enter (short versions are also
 * available, enter 'help<enter>' to see them):
 *
 * @li \c run run system until paused.
 * @li \c continue same as run
 * @li \c step advance system by one step.
 * @li \c pause pause(stop) system.
 * @li \c test call test functions.
 * @li \c quit shutdown system and quit.
 * @li \c help show help.
 */
	class BVSD
	{
	};
}

/** Main function, creates interactive loop. */
int main(int argc, char** argv)
{
	LOG(2, "starting!");
	bvs = new BVS::BVS(argc, argv);

	LOG(2, "loading modules!");
	bvs->loadModules();

	LOG(2, "connecting modules!");
	bvs->connectAllModules();

	LOG(2, "starting!");
	bvs->start();

	bvs->run();

	std::string input;
	while (input != "q" && input != "quit")
	{
		std::getline(std::cin, input);

		if (input == "r" || input == "run" || input == "c" || input == "continue")
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
			std::cout << "   r|run        run system until paused" << std::endl;
			std::cout << "   c|continue   same as run" << std::endl;
			std::cout << "   s|step       advance system by one step" << std::endl;
			std::cout << "   p|pause      pause(stop) system" << std::endl;
			std::cout << "   t|test       call test functions" << std::endl;
			std::cout << "   q|quit       shutdown system and quit" << std::endl;
			std::cout << "   h|help       show help" << std::endl;
		}
	}

	return 0;
}



/** Performs some logger tests.
 * This functions performs some tests on the logger system. Nothing fancy, can
 * be studied to gain some insight into using the logger system.
 */
int testLogger()
{
	LOG(0, "to CLI FILE");
	bvs->disableLogConsole();
	LOG(0, "to FILE only");

	BVS::Logger file("FILE LOG", 3, BVS::Logger::TO_FILE);
	file.out(0) << "FILE ONLY" << std::endl;

	bvs->enableLogConsole();
	BVS::Logger cli("CLI LOG", 3, BVS::Logger::TO_CLI);
	cli.out(0) << "CLI ONLY" << std::endl;

	bvs->disableLogConsole();
	bvs->disableLogFile();
	LOG(0, "NOOP");

	bvs->enableLogConsole();
	LOG(0, "to CLI");

	bvs->disableLogConsole();
	bvs->enableLogFile("BVSLog.txt", true);
	LOG(0, "to FILE AGAIN");

	bvs->enableLogConsole();
	BVS::Logger both("to BOTH", 0, BVS::Logger::TO_CLI_AND_FILE);
	both.out(0) << "to CLI AND FILE" << std::endl;

	return 0;
}



/** Performs some config tests.
 * This functions performs some tests on the config system. Nothing fancy, can
 * be studied to gain some insight into using the config system.
 */
int testConfig()
{
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
	for (auto& it : list)
	{
		(void) it;
		(void) count;
		LOG(0, count++ << ": " << it);
	}

	return 0;
}

