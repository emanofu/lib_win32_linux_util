#ifndef igs_resource_dir_list_h
#define igs_resource_dir_list_h

#include <string> // std::basic_string<TCHAR>
#include <vector> // std::vector
#include <set>	// std::set

#ifndef IGS_RESOURCE_IRW_EXPORT
# define IGS_RESOURCE_IRW_EXPORT
#endif

namespace igs {
 namespace resource {
  /* �ǥ��쥯�ȥ�ꥹ�Ȥ��ɤߡ�"std::set"������� */
  IGS_RESOURCE_IRW_EXPORT void dir_list(
	LPCTSTR dir_path
	,std::set< std::basic_string<TCHAR> >&set_list
  );
  /* �ǥ��쥯�ȥ�ꥹ�Ȥ��ɤߡ�"std::vector"�������
  std::sort(vec_list.begin(),vec_list.end());�����std::set��Ʊ������¤� */
  IGS_RESOURCE_IRW_EXPORT void dir_list(
	LPCTSTR dir_path
	,std::vector< std::basic_string<TCHAR> >&vec_list
  );
 }
}

#endif /* !igs_resource_dir_list_h */
