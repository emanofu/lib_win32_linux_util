#include <sys/types.h>	/* getuid(),getpwuid() */
#include <pwd.h>	/* getpwuid() */
#include <unistd.h>	/* getuid() */
#include <cerrno>	/* errno */
#include <iostream>	/* TCOUT */
#include <stdexcept> /* std::domain_error */
#include "igs_resource_msg_from_err.h"
#include "igs_resource_user_name.h"

/* アプリ内の共通文字定義 */
void igs::resource::user_name(std::basic_string<TCHAR>&name)
{
	/****** 環境変数からユーザー名をとる
	const char *const cp_user_env = "USER";
	igs::resource::getenv(cp_user_env,name);
	******/

	/* 現在のプロセスの実ユーザーIDからとる */
	const struct passwd *const tp_passwd = ::getpwuid(
		::getuid() /* 現在のプロセスの実ユーザーID */
	);
	if (0 == tp_passwd) {
		throw std::domain_error(igs_resource_msg_from_err(
			TEXT("getpwuid(-) tp_passwd is zero"),errno
		));
	}
	if ((0 == tp_passwd->pw_name) || ('\0' == tp_passwd->pw_name[0])) {
		throw std::domain_error(igs_resource_msg_from_er(
			TEXT("getpwuid(-).pw_name is not exist")
		));
	}
	name = igs::resource::ts_from_mbs( tp_passwd->pw_name );
#if defined DEBUG_IGS_RESOURCE_USER_NAME
	std::TCOUT << TEXT("domain_name \"") << name << TEXT("\"\n");
#endif

	/* ドメイン名からユーザー名のみ抜き出す
		Mac OS Xでは --> user@xxxxxx.xx.xx --> user
	*/
	std::basic_string<TCHAR>::size_type index = name.find('@');
	if (index != std::basic_string<TCHAR>::npos ) {
		name.erase(index);
	}
}
#if defined DEBUG_IGS_RESOURCE_USER_NAME
#include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (1 != argc) {
		std::cout << "Usage " << argv[0] << std::endl;
		return 1;
	}
  try {
	std::basic_string<TCHAR> name;
	igs::resource::user_name(name);
	std::TCOUT << TEXT("user_name \"") << name << TEXT("\"\n");
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_USER_NAME */
/*
# gcc(32/64bits) :61,63 w! tes_gcc_igs_rsc_user_name.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_USER_NAME -I. igs_resource_user_name.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_user_name_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_USER_NAME -DUNICODE -I. igs_resource_user_name.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_user_name_wc
*/
