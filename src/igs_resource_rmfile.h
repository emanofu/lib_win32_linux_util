#ifndef igs_resource_rmfile_h
#define igs_resource_rmfile_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_IRF_EXPORT
# define IGS_RESOURCE_IRF_EXPORT
#endif

namespace igs {
 namespace resource {
	/*
		�ե�����1�ĺ��
		�ե����뤬�ʤ�������Ǥ��ʤ����Ȥ������顼�㳰���ꤲ��
	*/

	IGS_RESOURCE_IRF_EXPORT void rmfile(LPCTSTR file_path);
 }
}

#endif /* !igs_resource_rmfile_h */
