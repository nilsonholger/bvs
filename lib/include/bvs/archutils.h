#ifndef BVS_ARCHUTILS_H
#define BVS_ARCHUTILS_H

#ifdef __unix__
#ifdef BVS_THREAD_NAMES
#include <sys/prctl.h>
#include <error.h>
#endif //BVS_THREAD_NAMES
#endif //__unix__



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** A utility function to set the thread name.
	 * NOTE: this only works on UNIX systems.
	 * @param[in] threadName Name to set (maximum length is 16 chars, this includes '\0')
	 * @return 'errno' from the prctl(...) syscall.
	 */
	int nameThisThread(const char* threadName);

	/**
	 * @todo add loader specific stuff (dlfcn.h), appropriate windows version...
	 */
} // namespace BVS



#endif //BVS_ARCHUTILS_H

