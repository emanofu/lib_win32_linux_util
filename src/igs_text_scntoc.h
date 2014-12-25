#ifndef igs_text_scntoc_h
#define igs_text_scntoc_h

#include "igs_os_type.h"

/*
	Softimage��".scntoc"�ե����뤫��
	���ϥե졼��Ƚ�λ�ե졼���ֹ������
	redners.exe�Τߤǻ��Ѥ���
*/
namespace igs {
 namespace text {
  void scntoc_start_end(
	const std::basic_string<TCHAR>&file_path // Extention is .scntoc
	,int&start_frame
	,int&end_frame
  );
 }
}
#endif /* !igs_text_scntoc_h */
