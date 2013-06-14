# BVS_ANDROID_APP
set(BVS_ANDROID_APP FALSE CACHE BOOL "Generate Android targets!")
mark_as_advanced(BVS_ANDROID_APP)

# BVS_GCC_VISIBILITY
set(BVS_GCC_VISIBILITY ON CACHE BOOL "Enable gcc's visibility feature (reduce dynamic library load times as well as decrease library size).")
mark_as_advanced(BVS_GCC_VISIBILITY)
if(BVS_GCC_VISIBILITY)
	add_definitions(-DBVS_GCC_VISIBILITY -fvisibility=hidden)
endif()

# BVS_LOG_SYSTEM
set(BVS_LOG_SYSTEM ON CACHE BOOL "Enable BVS builtin log system.")
if(BVS_LOG_SYSTEM)
	add_definitions(-DBVS_LOG_SYSTEM)
endif()

# BVS_MODULE_HOTSWAP
set(BVS_MODULE_HOTSWAP OFF CACHE BOOL "Enable BVS's HotSwap(TM;-) facilities. WARNING: DEVELOPERS ONLY!")
mark_as_advanced(BVS_MODULE_HOTSWAP)
if(BVS_MODULE_HOTSWAP)
	add_definitions(-DBVS_MODULE_HOTSWAP)
endif()

# BVS_STATIC_MODULES
set(BVS_STATIC_MODULES OFF CACHE BOOL "Enable compilation of static modules and creation of single library object!")
mark_as_advanced(BVS_STATIC_MODULES)
if(BVS_STATIC_MODULES)
	set(BVS_MODULE_TYPE STATIC)
	set(BVS_LINK_LIBRARIES "")
	add_definitions(-DBVS_STATIC_MODULES)
	set(ENV{BVS_STATIC_MODULES} "")
else()
	set(BVS_MODULE_TYPE SHARED)
	set(BVS_LINK_LIBRARIES bvs)
endif()

# BVS_THREAD_NAMES
set(BVS_THREAD_NAMES ${UNIX} CACHE BOOL "Enable thread naming (htop: enable 'Show custom thread names', or try 'ps -La'). UNIX ONLY!")
mark_as_advanced(BVS_THREAD_NAMES)
if(BVS_THREAD_NAMES)
	add_definitions(-DBVS_THREAD_NAMES)
endif()

# BVS_OSX_ANOMALIES
set(BVS_OSX_ANOMALIES ${APPLE} CACHE BOOL "Enable OSX anomalies, e.g. 'so->dylib'.")
mark_as_advanced(BVS_OSX_ANOMALIES)
if(BVS_OSX_ANOMALIES)
	add_definitions(-DBVS_OSX_ANOMALIES)
endif()

# DEPENDENCIES
add_dependency(BVS_STATIC_MODULES ON_IF BVS_ANDROID_APP)
add_dependency(BVS_MODULE_HOTSWAP OFF_IF BVS_STATIC_MODULES)
