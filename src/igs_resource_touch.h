#ifndef igs_resource_touch_h
#define igs_resource_touch_h

#if defined _WIN32 //-------------------------------------------------
# include <windows.h>
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
	/* modify����Τߤ򹹿����� */
	IGS_RESOURCE_IRF_EXPORT void touch(LPCTSTR file_path);
 }
}
#else //--------------------------------------------------------------
namespace igs {
 namespace resource {
	/* access��modify����򹹿����� */
	void touch(const char*file_path);
 }
}
#endif //-------------------------------------------------------------

#endif /* !igs_resource_touch_h */
