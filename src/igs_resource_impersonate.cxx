#include "igs_resource_msg_from_err.h"
#include "igs_resource_impersonate.h"

igs::resource::impersonate::impersonate(
	const std::basic_string<TCHAR>&user_name
	,const std::basic_string<TCHAR>&password
)	:hToken_(0)
{
/*	LogonUser(-)
	ローカルコンピュータにユーザーをログオンさせようとします。
	リモートコンピュータへのログオンはできません。
	ユーザー名とドメインを使ってユーザーを指定し、
	平文パスワードを使ってユーザーを認証します。
	この関数が成功すると、
	既にログオンしているユーザーを表すトークンのハンドル
	が取得できます。
	次に、このトークンハンドルを使って、
	指定したユーザーを偽装するか、
	多くの場合は指定したユーザーのコンテキスト内で動作する
	プロセスを作成できます。
*/
	if (0 == ::LogonUser( 
		user_name.c_str()		/* ユーザー名指定文字列   */
		,NULL		      /* ドメインまたはサーバー指定文字列 */
		,password.c_str()		/* パスワード指定文字列   */
		,LOGON32_LOGON_INTERACTIVE	/* ログオン動作タイプ指定 */

		/*
		LOGON32_LOGON_BATCH
			1385 : ログオン失敗: 要求された種類のログオンは、
			このコンピューターではユーザーに許可されていません。
		LOGON32_LOGON_SERVICE
			ImpersonateLoggedOnUser(-)の成功のあと、
			mkdir(-)でエラー
 		*/

		,LOGON32_PROVIDER_DEFAULT	/* ログオンプロバイダ指定 */
		,&this->hToken_			/* トークンハンドル受取り */
	)) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("LogonUser(-)"),::GetLastError()
		));
	}
/*	ImpersonateLoggedOnUser(-)
	呼び出し側のスレッドが、
	既にログオンしているユーザーのセキュリティコンテキスト
	を偽装できるようにします。
	そのユーザーをトークンハンドルで表します。
	(注意1)
		ユーザー設定や環境変数などの、
		ユーザープロファイルは引き継がない
	(注意2)
		Dialog等GUI表示を行うDesktopの集まりである、
		ウインドウステーションは引き継がない
*/
	if (0 == ::ImpersonateLoggedOnUser(
		/* 既にログオンしているユーザーを表すトークンのハンドル */
		this->hToken_
	)) {
		this->close_handle_();
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("ImpersonateLoggedOnUser(-)"),::GetLastError()
		));
	}
}
void igs::resource::impersonate::close_handle_(void) {
	/* 開いているオブジェクトハンドルを閉じます */
	if (0 == ::CloseHandle( this->hToken_ )) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("CloseHandle(-)") ,::GetLastError()
		));
	}
}
void igs::resource::impersonate::close(void) {
	/* クライアントアプリケーションによる偽装を終了します */
	if (0 == ::RevertToSelf()) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("RevertToSelf(-)") ,::GetLastError()
		));
	}
	this->close_handle_();
}
igs::resource::impersonate::~impersonate() {
	try {igs::resource::impersonate::close();} catch(...) {}
}

#if defined DEBUG_IGS_RESOURCE_IMPERSONATE
#include <iostream>
#include "igs_resource_msg_from_err.cxx"
#include "igs_resource_user_name.cxx"
namespace {
 std::basic_string<TCHAR> str;
 const std::basic_string<TCHAR>&user_name_(void) {
	igs::resource::user_name(str);
	return str;
 }
}
int main(int argc, char *argv[]) {
	argc;
	argv;

	std::basic_ostringstream<TCHAR> ost;
	ost << TEXT("user_name1=\"") << user_name_() << TEXT("\"\n");

	igs::resource::impersonate impe( TEXT("user1") ,TEXT("user1") );
	ost << TEXT("user_name2=\"") << user_name_() << TEXT("\"\n");
	impe.close();

	ost << TEXT("user_name3=\"") << user_name_() << TEXT("\"\n");

	std::TCOUT << ost.str(); 
	return 0;
}
#endif /* !DEBUG_IGS_RESOURCE_IMPERSONATE */
/*
rem vc2008sp1(32/64bits) :110,127 w! tes_msc_igs_rsc_impersonate.bat
set _DEB=DEBUG_IGS_RESOURCE_IMPERSONATE
set _SRC=igs_resource_impersonate
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_impersonate_mb
set _INC=.
set _LIB=advapi32.lib
cl /W4 /WX /MD /EHa /O2 /wd4819 /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _TGT=tes_msc_%PROCESSOR_ARCHITECTURE%_igs_rsc_impersonate_wc
cl /W4 /WX /MD /EHa /O2 /wd4819 /DUNICODE /D%_DEB% /I%_INC% %_SRC%.cxx %_LIB% /Fe%_TGT%
mt -manifest %_TGT%.exe.manifest -outputresource:%_TGT%.exe;1
del %_SRC%.obj %_TGT%.exe.manifest
set _LIB=
set _INC=
set _TGT=
set _SRC=
set _DEB=
# gcc(32/64bits) :681,682 w! tes_gcc_igs_rsc_impersonate.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_IMPERSONATE -I. igs_resource_impersonate.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_impersonate
*/
