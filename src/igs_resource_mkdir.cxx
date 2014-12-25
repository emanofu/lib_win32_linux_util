#if defined _WIN32 // win32 or win64
#include "igs_resource_mkdir_win.cxx"
#else
#include "igs_resource_mkdir_unix.cxx"
#endif
