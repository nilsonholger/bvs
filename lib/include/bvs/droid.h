#ifndef BVS_DROID_H
#define BVS_DROID_H
#include <stdio.h>
#include <string>
#include <stdlib.h>
//std::to_string not yet available in ndk, so overwrite function
namespace std
{
	string to_string(int i);
}
void exit(int);
#endif //BVS_DROID_H


