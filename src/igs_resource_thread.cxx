#if defined _WIN32 // win32 or win64
# include "igs_resource_thread_win.cxx"
#else
# include "igs_resource_thread_unix.cxx"
#endif
