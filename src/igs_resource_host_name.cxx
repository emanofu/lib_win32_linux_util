#if defined _WIN32 // win32 or win64
# include "igs_resource_host_name_win.cxx"
#else
# include "igs_resource_host_name_unix.cxx"
#endif
