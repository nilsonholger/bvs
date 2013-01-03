#include <csignal>
#include <execinfo.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "bvs/bvs.h"



BVS::BVS* bvs;
BVS::Logger logger("Daemon");



/** Signal function, handles occurence of several system signals.
 * This function handles occurence of signals passed on by itself (for example
 * through the shutdownFunction) or by the system (for example pressing
 * 'Ctrl-C') as well as prints a stack trace (upon segmentation faults).
 * @param[in] sig System signal.
 */
void mainSignal(int sig);

/** Shutdown function, called by BVS when shutdown requested. */
void shutdownFunction();

/** Performs some logger tests.
 * This functions performs some tests on the logger system. Nothing fancy, can
 * be studied to gain some insight into using the logger system.
 * @TODO create/integrate into some full blown test suite
 */
int testLogger();

/** Performs some config tests.
 * This functions performs some tests on the config system. Nothing fancy, can
 * be studied to gain some insight into using the config system.
 * @TODO create/integrate into some full blown test suite
 */
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

