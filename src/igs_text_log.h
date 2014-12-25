#ifndef igs_text_log_h
#define igs_text_log_h

#include <string> // std::basic_string<TCHAR>
#include "igs_os_type.h"

namespace igs {
 namespace text {
  namespace log {
   class parameters {
   public:
	parameters();
	LPCTSTR err_key;	// stderr
	LPCTSTR out_key;	// stdout
   };
   /*
	log_sw     == true�ʤ顢log��Ͽ��
	dbgview_sw == true�ʤ顢debugɽ��(Windows�Τ�)��
	�򤹤롣
	ξ��false�ξ��Ϥʤˤ⤷�ʤ���
	--- Windows debugɽ���Ȥ� ---
	igs::text::log::append_to_file(-)���
	::OutputDebugString(-)��¹Ԥ��Ƥ��롣
	--- Windows Service �� debug ����ˡ ---
	microsoft�Υ����Ȥ���download����"dbgview.exe"��
	������å���˥塼��"�����ԤȤ��Ƽ¹�..."��
	�¹Ԥ��ơ�����򸫤�
   */
   /* log�ν񼰤�̵�뤷��1�Ե�Ͽ���� */
   void append_to_file(
	 const std::basic_string<TCHAR>&message_with_no_return/* ���Դޤ� */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw = true	/* debugɽ���ؼ� */
	,const bool&log_sw = true	/* log��Ͽ�¹Իؼ� */
   );
   /* log�ν񼰤�1�Ե�Ͽ���� */
   void append_hms_key_msg_ret(
	 const std::basic_string<TCHAR>&category_word
	,const std::basic_string<TCHAR>&message_with_no_return/* ���Դޤ� */
	,const std::basic_string<TCHAR>&file_path
	,const bool&dbgview_sw = true	/* debugɽ���ؼ� */
	,const bool&log_sw = true	/* log��Ͽ�¹Իؼ� */
	,const bool&ymdhms_sw = false /* hms�Ǥʤ�ymdhms��ƹԤ˵�Ͽ�ؼ�
				critical error��"C:/temp/..."��¸�� */
   );
  }
 }
}

#endif /* !igs_text_log_h */
