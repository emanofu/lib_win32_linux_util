#ifndef igs_resource_rmdir_h
#define igs_resource_rmdir_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
	/*
		�ǥ��쥯�ȥ�1�ĺ��
			directory���ʤ�
			remove�Ǥ��ʤ�
			���Ǥʤ��ǥ��쥯�ȥ꤬�ä��ʤ�
		�Ȥ������顼�㳰���ꤲ��
	*/
	IGS_RESOURCE_IRF_EXPORT void rmdir(LPCTSTR dir_path);
 }
}

#endif /* !igs_resource_rmdir_h */
