#if defined _WIN32 // x32 or x64
# include "igs_resource_cpu_rate_win.cxx"
#else
# include "igs_resource_cpu_rate_unix.cxx"
#endif
