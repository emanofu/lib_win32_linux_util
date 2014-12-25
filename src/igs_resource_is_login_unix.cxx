#include <iostream>
#include <iomanip>
#include <utmp.h>

#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
namespace {
 void utmp_record_print_(int count,struct utmp *utp) {
	std::cout
		<< std::setw(2) << count;
	if (USER_PROCESS != utp->ut_type) {std::cout << std::endl; return;}
	std::cout
		<< "\tlogin_type=" << utp->ut_type
		<< " login_process_pid=" << utp->ut_pid
		<< " device_name=" << utp->ut_line
		<< "\n\tid_or_abbrev="
			<< ((0==utp->ut_id[0])?'-':utp->ut_id[0])
			<< ((0==utp->ut_id[1])?'-':utp->ut_id[1])
			<< ((0==utp->ut_id[2])?'-':utp->ut_id[2])
			<< ((0==utp->ut_id[3])?'-':utp->ut_id[3])
		<< ' ' << " session_id=" << utp->ut_session
		<< " user=" << "\"" << utp->ut_user << "\""
		<< "\n\thost=" << "\"" << utp->ut_host << "\""
		<< std::endl
		;
 }
}
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */

#include "igs_resource_is_login.h"

bool igs::resource::is_login(void) {
/* /var/run/utmp¤òÆÉ¤à... */
	::setutent();
	int user_process_count = 0;
	int console_login_count = 0;
	struct utmp *utp = 0;
	for (int ii=0; (utp = ::getutent())!=0; ++ii) {
#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
		utmp_record_print_(ii,utp);
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */
		if (USER_PROCESS == utp->ut_type) {
			++user_process_count;
			if ('\0' != utp->ut_host[0]) {
				++console_login_count;
			}
		}
	}
	::endutent();
#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
		std::cout << "user_process_count="
			   << user_process_count << std::endl;
		std::cout << "console_login_count="
			   << console_login_count << std::endl;
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */

	return 0 != console_login_count;
}

#if defined DEBUG_IGS_RESOURCE_IS_LOGIN
int main(int argc,char *argv[])
{
  try {
	if ( igs::resource::is_login() ) {
		std::cout << "log on";
	} else {
		std::cout << "log off";
	}
		std::cout << std::endl;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_IS_LOGIN */
/*
# gcc(32/64bits)  :76,77 w! tes_gcc_igs_rsc_is_login.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_IS_LOGIN -I. igs_resource_is_login.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_is_login
*/
