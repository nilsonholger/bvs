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



/** BVSD namespace, contains only the bvs daemon. */
namespace BVSD
{
/** BVS framework's command line daemon.
 * This is a simple command line daemon. It serves as a client to the BVS
 * framework. It is also intended as a sample client.
 *
 * It is interactive, i.e., it forks the framework into its own thread. You can
 * then use the following commands by just entering them on the command line
 * and then pressing enter (short versions are also available, enter
 * 'help<enter>' to see them):
 *
 * @li \c run run system until paused.
 * @li \c continue same as run
 * @li \c step advance system by one step.
 * @li \c pause pause(stop) system.
 * @li \c test call test functions.
 * @li \c quit shutdown system and quit.
 * @li \c help show help.
 *
 * If you do not want the client to fork the framework and become interactive,
 * you can set the following in BVS' Config file:
 * @code
 * [BVSD]
 * interactive = OFF
 * @endcode
 */
	class BVSD
	{
	};
}

