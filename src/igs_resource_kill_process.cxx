#if defined _WIN32 // win32 or win64
#include "igs_resource_kill_process_win.cxx"
#else
#include "igs_resource_kill_process_unix.cxx"
#endif
