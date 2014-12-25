#ifndef igs_resource_wnet_h
#define igs_resource_wnet_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>	// LPTSTR
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
  namespace wnet {
   IGS_RESOURCE_IRF_EXPORT void connect(
	LPTSTR local_name	// "G:"
	,LPTSTR remote_name	// "\\host1\Users\Public"
   );
   IGS_RESOURCE_IRF_EXPORT void connect_if_not_exist(
	LPTSTR local_name	// "G:"
	,LPTSTR remote_name	// "\\host1\Users\Public"
	,LPTSTR target_path	// "G:\irfm"
   );
   IGS_RESOURCE_IRF_EXPORT void disconnect(
	LPTSTR local_name	// "N:"
   );
  }
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_wnet_h */
