#include <unistd.h> // ::gethostname(-), HOST_NAME_MAX
#include <cerrno> // ::errno
#include <stdexcept> // std::domain_error
#include "igs_resource_msg_from_err.h"
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255 // for Mac OS X 10.4.7  Darwin 8.7.0
#endif
namespace {
 void host_name_unix_(char *hostname, int namelen) {
	/* �ۥ���̾��OS��API����Ȥ롢
		�Ķ��ѿ�("HOST"or"HOSTNAME")�ϡ�
		Mac OS X 10.4.7�ˤ����ơ�
		¾�ޥ��󤫤�'slogin'�������ä�����
		���ꤵ��ʤ�
 	*/
	/* HOST_NAME_MAX(=namelen)�ˤĤ���(rhel4��`man 2 gethostname`���)
		���	SUSv2 �Ǥ�
			�֥ۥ���̾�� 255 �Х��Ȥ����¤�����
		���Ȥ��ݾڤ��Ƥ��롣
		POSIX 1003.1-2001 �Ǥ�
			�֥ۥ���̾(��ü�Υ̥�ʸ���ϴޤޤʤ�)��
			HOST_NAME_MAX �Х��Ȥ����¤�����
		���Ȥ��ݾڤ��Ƥ��롣
	*/
	if (0 != ::gethostname(hostname,namelen)) {
		std::basic_string<TCHAR> str(TEXT("gethostname(-):"));
		str += igs::resource::ts_from_mbs( ::strerror(errno) );
		throw std::domain_error(igs_resource_msg_from_er(str));
	}
	hostname[namelen-1] = '\0';
 }
}
//--------------------------------------------------------------------
#include <cctype> /// tolower()
#include <string> // std::string
#include "igs_resource_host_name.h"
void igs::resource::host_name(std::basic_string<TCHAR>&name)
{
#if 0  //------ comment out ------
	/* �Ķ��ѿ�����ۥ���̾��Ȥ�...�ΤϤ��ʤ� */
# if defined __GNUC_RH_RELEASE__ // rhel4
	igs::resource::getenv("HOST", name);
	//igs::resource::getenv("HOSTNAME", name);
# elif defined __APPLE_CC__ // darwin8(Mac OS X 10.4.7)
	/*
		¾�ޥ��󤫤�'slogin'�������ä�����
		�Ķ��ѿ�("HOST"or"HOSTNAME")�����ꤵ��ʤ�
 	*/
# else
# endif
#endif //------ comment out ------

	/* �ۥ���̾�Ȥ� */
	char hostname[HOST_NAME_MAX]; hostname[0] = 0;
	host_name_unix_(hostname,sizeof(hostname));
	hostname[HOST_NAME_MAX-1] = 0;
	name = igs::resource::ts_from_mbs( hostname );

	/* �ۥ���̾���Ȥ�ʤ��Ȥ��ϥ��顼 */
	if (name.empty()) {
		throw std::domain_error(igs_resource_msg_from_er(
			TEXT("Can not get host_name_unix_")
		));
	}

	/* �ɥᥤ��̾����ۥ���̾��������ʬ�Τߤˤ���
	darwin8	--> Host-MAC.local		--> Host-MAC
	rhel5	--> hostrhel5.net1.host1.jp	--> hostrhel5
	*/
	std::string::size_type index = name.find('.');
	if (std::string::npos != index) { name.erase(index);
	}
}
#if defined DEBUG_IGS_RESOURCE_HOST_NAME
#include <iostream> // std::cout
#include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (1!=argc) { std::cout << "Usage:" << argv[0] << "\n"; return 1; }
  try { 
	char hostname_mb[HOST_NAME_MAX]; hostname_mb[0] = 0;
	host_name_unix_(hostname_mb,sizeof(hostname_mb));

	std::basic_string<TCHAR> hostname_tc;
	igs::resource::host_name(hostname_tc);

	std::cout  <<      "1.host_name_win_(-) \""  << hostname_mb
							<< "\"\n";
	std::TCOUT << TEXT("2.host_name(-)      \"") << hostname_tc
							<< TEXT("\"\n");
  }
  catch (std::exception& e) {std::cerr<<"exception=\""<<e.what()<<"\"\n";}
  catch (...) { std::cerr << "other exception\n"; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_HOST_NAME */
/*
# gcc(32/64bits)  :108,110 w! tes_gcc_igs_rsc_host_name.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_HOST_NAME -I../src igs_resource_host_name_unix.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_host_name_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_HOST_NAME -DUNICODE -I../src igs_resource_host_name_unix.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_host_name_wc
*/
