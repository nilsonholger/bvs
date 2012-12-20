#ifndef BVS_ARCHUTILS_H
#define BVS_ARCHUTILS_H

#include <string>

#include "bvs/traits.h"



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** A utility function to set the thread name.
	 * NOTE: this only works on UNIX systems.
	 * @param[in] threadName Name to set (maximum length is 12 chars, this includes '\0'), will be prefixed with 'bvs:'
	 * @return 'errno' from the prctl(...) syscall.
	 */
	BVS_PUBLIC int nameThisThread(std::string threadName);
} // namespace BVS



#endif //BVS_ARCHUTILS_H

