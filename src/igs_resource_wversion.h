#ifndef igs_resource_wversion_h
#define igs_resource_wversion_h

#include <windows.h> // TCHAR
#include <string> // std::basic_string<...>

# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  /* for ver 5.0.0(Windows2000) or later */
  void wversion(
	int&major_ver
	,int&minor_ver
	,int&build_number
	,std::basic_string<TCHAR>&ver_str
  );
 }
}

#endif /* !igs_resource_wversion_h */
