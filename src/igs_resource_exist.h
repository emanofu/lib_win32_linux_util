#ifndef igs_resource_exist_h
#define igs_resource_exist_h

#include "igs_os_type.h"

# ifndef IGS_RESOURCE_LOG_EXPORT
#  define IGS_RESOURCE_LOG_EXPORT
# endif

namespace igs {
 namespace resource {
  namespace exist {
/*
	dir���뤤��file��¸�ߤ��ǧ���롣
		����ͤ϶��̤ǡ�
			true	¸�ߤ���
			false	¸�ߤ��ʤ�
		���顼�ξ����㳰���ꤲ��
*/
	/* file���뤤��dir��¸�� */
	IGS_RESOURCE_LOG_EXPORT const bool file_or_directory(LPCTSTR path);

	/* file��¸��(dir���Ф��Ƥ�false���֤�) */
	IGS_RESOURCE_LOG_EXPORT const bool file(LPCTSTR path);

	/* dir��¸��(file���Ф��Ƥ�false���֤�) */
	IGS_RESOURCE_LOG_EXPORT const bool directory(LPCTSTR path);
  }
 }
}

#endif /* !igs_resource_exist_h */
