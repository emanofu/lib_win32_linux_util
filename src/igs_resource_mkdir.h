#ifndef igs_resource_mkdir_h
#define igs_resource_mkdir_h

#include "igs_os_type.h"

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

namespace igs {
 namespace resource {
	/* �ǥ��쥯�ȥ�1�ĺ�����¸�ߤ��ǧ�����֤롢¹��ľ�ܤϤĤ���ʤ� */
	IGS_RESOURCE_LOG_EXPORT void mkdir_certain(
		LPCTSTR dir_path
		,DWORD wait_milli_seconds=10	/* 0.01�� */
		,int wait_times=1000		/* 10�� */
	);

	/* �ǥ��쥯�ȥ�򷡤�ʤࡢ¹�ʲ������ˤ�ꤹ�٤ƺ�� */
	IGS_RESOURCE_LOG_EXPORT void mkdir_descendant(LPCTSTR dir_path);

	/* lock���뤿��˥ǥ��쥯�ȥ�1�ĺ���(¹��ľ�ܤϤĤ���ʤ�)
		Windows�Ǥ�����ͤ�
			ERROR_SUCCESS		=0   ��������
			ERROR_ALREADY_EXISTS	=183 ����¸�ߤ���
			ERROR_ACCESS_DENIED	=5   ������������
		unix�Ǥ�����ͤ�
			0	��������
			EEXIST	����¸�ߤ��Ƥ���
		����ʳ��ϥ��顼��throw����
	*/
	IGS_RESOURCE_LOG_EXPORT DWORD mkdir_for_lock(LPCTSTR dir_path);
 }
}

#endif /* !igs_resource_mkdir_h */
