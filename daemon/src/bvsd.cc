#include "bvsd.h"

/** Main function, creates interactive loop. */
int main(const int argc, const char** argv)
{
	signal(SIGINT, mainSignal);
	signal(SIGSEGV, mainSignal);
	signal(SIGALRM, mainSignal);

	//bvs = new BVS::BVS(argc, argv, &shutdownFunction); // TODO: disable shutdown function for now, also see mainSignal
	bvs = new BVS::BVS(argc, argv);

	LOG(2, "loading modules!");
	bvs->loadModules();

	LOG(2, "connecting modules!");
	bvs->connectAllModules();

	LOG(2, "starting!");
	bvs->start();

	bvs->run();

	std::string input;
	while (true)
	{
		std::getline(std::cin, input);
		if (std::cin.eof()) input = "q";

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
		else if (input.substr(0,2) == "hs" || input.substr(0, 7) == "hotswap")
		{
			size_t delimiter = input.find_first_of(" ");
			input.erase(0, delimiter+1);
			if (input.empty() || delimiter==std::string::npos) std::cout << "ERROR: no module ID given!" << std::endl;
			else bvs->hotSwap(input);
		}
		else if (input == "q" || input == "quit")
		{
			LOG(2, "quitting...");
			bvs->quit();
			LOG(2, "finished!");
			break;
		}
		else if (input == "h" || input == "help")
		{
			std::cout << "usage:" << std::endl;
			std::cout << "   r|run            run system until paused" << std::endl;
			std::cout << "   c|continue       same as run" << std::endl;
			std::cout << "   s|step           advance system by one step" << std::endl;
			std::cout << "   p|pause          pause(stop) system" << std::endl;
			std::cout << "   hs|hotswap <arg> HotSwap(TM) <moduleID>" << std::endl;
			std::cout << "   q|quit           shutdown system and quit" << std::endl;
			std::cout << "   h|help           show help" << std::endl;
		}
		else
		{
			std::cout << ">>> unknown command: " << input << std::endl << ">>> for help press 'h<enter>'!" << std::endl;
		}
	}

	delete bvs;

	return 0;
}



void mainSignal(int sig)
{
	switch (sig)
	{
		case SIGINT:
			LOG(2, "Caught 'Ctrl-C', quitting!");
			break;
		case SIGSEGV:
			LOG(2, "Caught segmentation fault...!");
			void *msgs[100];
			size_t size;
			size = backtrace(msgs, 100);
			fprintf(stderr, "\n\n\nCaught signal %d (%s)!\n\n", sig, strsignal(sig));
			char** messages;
			messages = backtrace_symbols(msgs, size);
			for (size_t i=2; i < size && messages != NULL; ++i)
				fprintf(stderr, "[bt]: (%lu) %s\n", i-2, messages[i]);
			free(messages);
			break;
		case SIGALRM:
			LOG(2, "Shutdown requested, quitting!");
			break;
	}

	if (sig==SIGSEGV)
	{
		signal(SIGSEGV, SIG_DFL);
		raise(SIGSEGV);
	}
	else
	{
		// TODO: BUG, causes system to hang, probably because it waits for some thread to join, investigate!
		bvs->quit();
		delete bvs;
		exit(EXIT_FAILURE);
	}
}



void shutdownFunction()
{
	LOG(1,"daemon exit caused by bvs shutdown request!");
	bvs->quit();
	alarm(1); // SIGALRM after 1 second so all framework/module threads have a chance to quit
}

