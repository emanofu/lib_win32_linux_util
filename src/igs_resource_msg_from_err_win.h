#ifndef igs_resource_msg_from_err_h
#define igs_resource_msg_from_err_h

#include <string>
#include "igs_os_type.h" /* windows.h... */

#define igs_tostr(n) igs_tostr_(n)	/* __LINE__ -->  56   */
#define igs_tostr_(n) #n		/* 56       --> "56" */

#ifndef IGS_RESOURCE_LOG_EXPORT
# define IGS_RESOURCE_LOG_EXPORT
#endif

/*------ ʸ���������Ѵ� --------------------------------------------*/
namespace igs {
 namespace resource {
  /*--- locale�����ܤ����ꤷ���ܸ�򰷤����Ȥ�ؼ� ---*/
  IGS_RESOURCE_LOG_EXPORT void locale_to_jp(void);
  /*--- �ޥ���Х���ʸ���� --> �磻��ʸ��ʸ���� ---*/
  IGS_RESOURCE_LOG_EXPORT void mbs_to_wcs(
	const std::string&mbs
	,std::wstring&wcs
	,const UINT code_page = CP_THREAD_ACP
  );
  /*--- �磻��ʸ��ʸ���� --> �ޥ���Х���ʸ���� ---*/
  IGS_RESOURCE_LOG_EXPORT void wcs_to_mbs(
	const std::wstring&wcs
	,std::string&mbs
	,const UINT code_page = CP_THREAD_ACP
  );
/*
	2012-08-22 �Ƥ�¦��̾�����ѹ�
	from_mbcs(-) --> ts_from_mbs(-)
	mbcs_from(-) --> mbs_from_ts(-)
*/
  /*--- UNICODE����ʤ�ޥ���Х���ʸ�����磻��ʸ��ʸ������Ѵ� ---*/
  const std::basic_string<TCHAR> ts_from_mbs( const std::string&mbs
	,const UINT code_page = CP_THREAD_ACP
  );
  /*--- UNICODE����ʤ�磻��ʸ��ʸ�����ޥ���Х���ʸ������Ѵ� ---*/
  const std::string mbs_from_ts( const std::basic_string<TCHAR>&ts
	,const UINT code_page = CP_THREAD_ACP
  );
  /*--- cp932��ޤ�ʸ�����utf-8���Ѵ�(mbcs�Τ�) ---*/
  const std::string utf8_from_cp932_mb( const std::string&text );
  /*--- utf-8��ޤ�ʸ�����cp932���Ѵ�(mbcs�Τ�) ---*/
  const std::string cp932_from_utf8_mb( const std::string&text );
 }
}

/*------ ���顼��å�����ɽ�� --------------------------------------*/
namespace igs {
 namespace resource {
  const std::string msg_from_err_( /* ľ�ˤ��ǤϤ����ʤ� */
	const std::basic_string<TCHAR>&tit
	,const DWORD error_message_id
	,const std::basic_string<TCHAR>&file
	,const std::basic_string<TCHAR>&line
	,const std::basic_string<TCHAR>&funcsig
	,const std::basic_string<TCHAR>&comp_type
	,const std::basic_string<TCHAR>&msc_full_ver
	,const std::basic_string<TCHAR>&date
	,const std::basic_string<TCHAR>&time
  );
 }
}
/*--- ::GetLastError()�ͤ��饨�顼��å����������� ---*/
#define igs_resource_msg_from_err(tit,error_message_id) igs::resource::msg_from_err_(tit,error_message_id,TEXT(__FILE__),TEXT(igs_tostr(__LINE__)),TEXT(__FUNCSIG__),TEXT(igs_tostr_(_MSC_VER)),TEXT(igs_tostr(_MSC_FULL_VER)),TEXT(__DATE__),TEXT(__TIME__))
/* igs_resource_msg_from_err("tmp",::GetLastError()) --> �¹Է���� --> "igs_resource_msg_from_err.cxx:58:_MSC_VER:150030729:2012:Jan:30:15:37:59" "int __cdecl main(int,char *[])" "tmp:���ꤵ�줿�⥸�塼�뤬���Ĥ���ޤ���" */
/*--- ���顼��å����������� ---*/
#define igs_resource_msg_from_er(tit                  ) igs::resource::msg_from_err_(tit,NO_ERROR        ,TEXT(__FILE__),TEXT(igs_tostr(__LINE__)),TEXT(__FUNCSIG__),TEXT(igs_tostr_(_MSC_VER)),TEXT(igs_tostr(_MSC_FULL_VER)),TEXT(__DATE__),TEXT(__TIME__))
/* igs_resource_msg_from_er("tmp")                   --> �¹Է���� --> "igs_resource_msg_from_err.cxx:58:_MSC_VER:150030729:2012:Jan:30:15:37:59" "int __cdecl main(int,char *[])" "tmp" */
#endif /* !igs_resource_msg_from_err_h */
