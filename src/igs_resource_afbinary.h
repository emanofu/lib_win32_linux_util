#ifndef igs_resource_afbinary_h
#define igs_resource_afbinary_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>
# ifndef IGS_RESOURCE_LOG_EXPORT
#  define IGS_RESOURCE_LOG_EXPORT
# endif
namespace igs {
 namespace resource {
  IGS_RESOURCE_LOG_EXPORT void append_no_sharing_file_binary(
	LPCTSTR file_path
	,LPCVOID lpBuffer
	,const DWORD nNumberOfBytesToWrite
	,const DWORD sleep_milliseconds=3	//=3...7 about log at once
	,const DWORD time_out_milli_seconds=1000//=1sec
  );
 }
}
#else //--------------------------------------------------------------
namespace igs {
 namespace resource {
  void append_atomic_file_binary(
	const char *file_path
	,const std::string&str
  );
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_afbinary_h */
