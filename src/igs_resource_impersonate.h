#ifndef igs_resource_impersonate_h
#define igs_resource_impersonate_h

#if defined _WIN32 //-------------------------------------------------

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

#include <windows.h>
#include <string>

namespace igs {
 namespace resource {
	class IGS_RESOURCE_IRF_EXPORT impersonate {
	public:
		impersonate(
			const std::basic_string<TCHAR>&user_name
			,const std::basic_string<TCHAR>&password
		);
		void close(void);
		~impersonate();
	private:
		impersonate();
		HANDLE hToken_;
		void close_handle_(void);
	};
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_impersonate_h */
