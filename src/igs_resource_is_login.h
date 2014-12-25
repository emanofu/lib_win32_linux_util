#ifndef igs_resource_is_login_h
#define igs_resource_is_login_h

#if defined _WIN32
# ifndef IGS_RESOURCE_IRF_EXPORT
#  define IGS_RESOURCE_IRF_EXPORT
# endif
namespace igs {
 namespace resource {
	IGS_RESOURCE_IRF_EXPORT bool is_login(void);
 }
}
#else
namespace igs {
 namespace resource {
	bool is_login(void);
 }
}
#endif

#endif /* !igs_resource_is_login_h */
