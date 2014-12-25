#ifndef igs_resource_msg_from_err_h
#define igs_resource_msg_from_err_h

#include <string>
#include "igs_os_type.h" /* TCHAR,TEXT(),... */

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
  );
  /*--- �磻��ʸ��ʸ���� --> �ޥ���Х���ʸ���� ---*/
  IGS_RESOURCE_LOG_EXPORT void wcs_to_mbs(
	const std::wstring&wcs
	,std::string&mbs
  );
/*
	2012-08-22 �Ƥ�¦��̾�����ѹ�
	from_mbcs(-) --> ts_from_mbs(-)
	mbcs_from(-) --> mbs_from_ts(-)
*/
  /*--- UNICODE����ʤ�ޥ���Х���ʸ�����磻��ʸ��ʸ������Ѵ� ---*/
  const std::basic_string<TCHAR> ts_from_mbs( const std::string&mbs
  );
  /*--- UNICODE����ʤ�磻��ʸ��ʸ�����ޥ���Х���ʸ������Ѵ� ---*/
  const std::string mbs_from_ts( const std::basic_string<TCHAR>&ts
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
	,const int erno
	,const std::string&file
	,const std::string&line
	,const std::string&pretty_function
	,const std::string&comp_type
	,const std::string&gnuc
	,const std::string&gnuc_minor
	,const std::string&gnuc_patchlevel
	,const std::string&gnuc_rh_release
	,const std::string&date
	,const std::string&time
  );
 }
}
/*--- errno�ͤ��饨�顼��å����������� ---*/
#define igs_resource_msg_from_err(tit,erno) igs::resource::msg_from_err_(tit,erno,__FILE__,igs_tostr(__LINE__),__PRETTY_FUNCTION__,igs_tostr_(__GNUC__),igs_tostr(__GNUC__),igs_tostr(__GNUC_MINOR__),igs_tostr(__GNUC_PATCHLEVEL__),igs_tostr(__GNUC_RH_RELEASE__),__DATE__,__TIME__)
/* igs_resource_msg_from_err("tmp",errno) --> �¹Է���� --> "igs_resource_msg_from_err.cxx:86:__GNUC__:4.1.2-48:2012:Jan:30:15:36:27" "int main(int, char**)" "tmp:No such file or directory" */
/*--- ���顼��å����������� ---*/
#define igs_resource_msg_from_er(tit      ) igs::resource::msg_from_err_(tit,0   ,__FILE__,igs_tostr(__LINE__),__PRETTY_FUNCTION__,igs_tostr_(__GNUC__),igs_tostr(__GNUC__),igs_tostr(__GNUC_MINOR__),igs_tostr(__GNUC_PATCHLEVEL__),igs_tostr(__GNUC_RH_RELEASE__),__DATE__,__TIME__)
/* igs_resource_msg_from_er("tmp")        --> �¹Է���� --> "igs_resource_msg_from_err.cxx:86:__GNUC__:4.1.2-48:2012:Jan:30:15:36:27" "int main(int, char**)" "tmp" */
#endif /* !igs_resource_msg_from_err_h */
