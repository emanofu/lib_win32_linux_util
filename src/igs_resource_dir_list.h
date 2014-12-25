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
  /* ディレクトリリストを読み、"std::set"に入れる */
  IGS_RESOURCE_IRW_EXPORT void dir_list(
	LPCTSTR dir_path
	,std::set< std::basic_string<TCHAR> >&set_list
  );
  /* ディレクトリリストを読み、"std::vector"に入れる
  std::sort(vec_list.begin(),vec_list.end());すればstd::setと同じ順で並ぶ */
  IGS_RESOURCE_IRW_EXPORT void dir_list(
	LPCTSTR dir_path
	,std::vector< std::basic_string<TCHAR> >&vec_list
  );
 }
}

#endif /* !igs_resource_dir_list_h */
