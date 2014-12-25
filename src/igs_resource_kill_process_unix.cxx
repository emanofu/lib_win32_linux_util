#include <stdexcept>	// std::domain_error()
//#include <signal.h>	// killpg()
#include <unistd.h>	// getpgid()
#include <cerrno>	// errno
#include "igs_resource_msg_from_err.h"

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
#include <iostream>	// std::cout
#include <iomanip>	// std::set()
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */

#include "igs_resource_kill_process.h"

namespace {
 void send_killpg_signal_(
	const pid_t top_pid
	,const int sig
	,const char *mname
 ) {
	/* ���ꤵ�줿�ץ����Υץ��������롼��ID ���֤� */
	const pid_t pgid = ::getpgid(top_pid);
	if (pgid < 0) {
		std::domain_error(igs_resource_msg_from_err(
			"getpgid(-)",errno
		));
	}
#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
	std::cout
		<< "killpg(-)"
		<< " top_pid=" << top_pid
		<< " pgid=" << pgid
		<< " sig=" << sig
		<< " mname=\"" << mname << "\""
		<< std::endl
		;
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */

	/* ���ꤷ�������ʥ����ꤷ���ץ������롼�פ�����
	killpg()��Ȥ��Ȼҥץ����ޤǽ�λ����
	debug�Ѥ�ɽ�����Ƥ��������ƥ��˥������Ĥ��Ƥ��ޤ��������� 2011-5-3
	*/
	if (0 != ::killpg(pgid,sig)) {
		std::domain_error(igs_resource_msg_from_err( mname,errno ));
	}
 }
}
void igs::resource::killpg_sigterm(const pid_t pid) {
	/* 1. �ޤ���λ(termination)�����ʥ������ */
	send_killpg_signal_( pid, SIGTERM, "killpg(,SIGTERM)" );
}
void igs::resource::killpg_sigint(const pid_t pid) {
	/* 2, SIGTERM�ǻ����Ǥ��ʤ����ϡ�
	�����ܡ��ɤ���γ�����(Interrupt)�����ʥ������ */
	send_killpg_signal_( pid, SIGINT, "killpg(,SIGINT)" );
}
void igs::resource::killpg_sigkill(const pid_t pid) {
	/* 3. SIGTERM,SIGINT�ǻ����Ǥ��ʤ����(����ӥץ���)�ϡ�
	��������롣���������ץ����ϡ��׻����Ǥ��ʤ� */
	send_killpg_signal_( pid, SIGKILL, "killpg(,SIGKILL)" );
}

/*---killpg()�����ƺѤळ�Ȥ����Τˤʤä��������� ��������--------*/
#include <sys/types.h>	// kill()
#include <signal.h>	// kill()
#include "igs_resource_tree_of_process_unix.cxx"
namespace {
#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
 void pr_pid_unix_(
	const int depth
	, const int now
	, const int all
	, const pid_t pid
	, const std::basic_string<TCHAR>&pname
 ) {
	for (int ii = 0; ii < depth; ++ii) { std::cout << ">"; }
	std::basic_ostringstream<TCHAR> ost;
	ost
		<< std::setw(3) << now
		<< std::setw(3) << all
		<< TEXT(" pid=") << std::setw(4) << pid
		<< TEXT(" pname=\"") << pname << TEXT("\"")
		<< std::endl
		;
	std::cout << igs::resource::mbs_from_ts( ost.str() );
 }
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
 void send_kill_signal_(
	const pid_t top_pid
	,const int sig
	,const char *mname
 ) {
	std::vector<pid_t> ptree;
	std::vector< std::basic_string<TCHAR> > pname;
	std::vector<int> pdept;
	::tree_of_process_unix( top_pid, ptree, pname, pdept );

/*	2010-10-19
	�ƥ��Ȥ��Ƥߤ�ȡ�
	tree_of_process_unix(-)�ڤ�kill(-)�ǡ�
	�ҥץ�������Ƥؽ�˥����ʥ��������ˡ���ȡ�
	�Ƥ��Ҥɤ������������ˤ��κ����ץ������Ĥ��礬������ᡣ
	killpg()��Ȥ��Ȼҥץ����ޤǽ�λ����
*/
/*
-topic  2011-05-03  unix�Υץ���tree��λ������Ȥ���
	�Ҷ��Υץ�������Ƥν�˽�λ������ȡ�
		�ƥץ�����λ���������˻ҥץ�������������ǽ�������ꡢ
		���������ҥץ���������ӤȤʤ뤫�⤷��ʤ�-->NG
		���뤤�ϡ��ҥץ����ν�λ��wait���Ƥ��ơ�
		�Ƥ�λ���������ˡ��Ƥ���ư��λ���뤫�⤷��ʤ�-->NG
	�ƤΥץ�������Ҷ��ν�˽�λ������ȡ�
		�ҥץ�����init�λҥץ����Ȥʤ�init��wait����-->OK?
	�ƤΥץ������롼�׽�λ������ȡ�
		tree���Τ���λ����-->OK
*/
	/* �ƤΥץ������� */
	//for (unsigned pnum = 0; pnum < ptree.size(); ++pnum) {
	for (int pnum=static_cast<int>(ptree.size())-1; 0<=pnum; --pnum) {
#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
 pr_pid_unix_(pdept.at(pnum),pnum+1,ptree.size(),ptree.at(pnum),pname.at(pnum) );
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
		if (0 != ::kill( ptree.at(pnum), sig )) {
			std::domain_error(igs_resource_msg_from_err(
				mname,errno
			));
		}
	}
 }
}
void igs::resource::kill_sigterm(const pid_t pid) {
	/* 1. �ޤ���λ(termination)�����ʥ������ */
	send_kill_signal_( pid, SIGTERM, "kill(,SIGTERM)");
}
void igs::resource::kill_sigint(const pid_t pid) {
	/* 2, SIGTERM�ǻ����Ǥ��ʤ����ϡ�
	�����ܡ��ɤ���γ�����(Interrupt)�����ʥ������ */
	send_kill_signal_( pid, SIGINT, "kill(,SIGINT)");
}
void igs::resource::kill_sigkill(const pid_t pid) {
	/* 3. SIGTERM,SIGINT�ǻ����Ǥ��ʤ����(����ӥץ���)�ϡ�
	��������롣���������ץ����ϡ��׻����Ǥ��ʤ� */
	send_kill_signal_( pid, SIGKILL, "kill(,SIGKILL)");
}
/*---killpg()�����ƺѤळ�Ȥ����Τˤʤä��������� �����ޤ�--------*/

#if defined DEBUG_IGS_RESOURCE_KILL_PROCESS
# include "igs_resource_msg_from_err_unix.cxx"
# include "igs_resource_dir_list_unix.cxx"
namespace {
 enum POST_TYPE {
	PG_SIG_TERM=0
	, PG_SIG_INT
	, PG_SIG_KILL
	, SIG_TERM
	, SIG_INT
	, SIG_KILL
 };
}
int main(int argc,char *argv[])
{
  try {
	if (3 != argc) {
		std::cout << "Usage : " << argv[0]
			<< " -gst/-gsi/-gsk/-st/-si/-sk pid\n"
			<< "-gst : Group SIGTERM(default)(kill process tree)\n"
			<< "-gsi : Group SIGINT(kill process tree)\n"
			<< "-gsk : Group SIGKILL(kill process tree)\n"
			<< "-st  : SIGTERM\n"
			<< "-si  : SIGINT\n"
			<< "-sk  : SIGKILL\n"
			<< "Be use -gst/-gsi/-gsk!\n"
			;
		return 1;
	}
	POST_TYPE type = PG_SIG_TERM;

	if (!strcmp("-gst", argv[1])) { type = PG_SIG_TERM; }
	else if (!strcmp("-gsi", argv[1])) { type = PG_SIG_INT; }
	else if (!strcmp("-gsk", argv[1])) { type = PG_SIG_KILL; }
	else if (!strcmp("-st", argv[1])) { type = SIG_TERM; }
	else if (!strcmp("-si", argv[1])) { type = SIG_INT; }
	else if (!strcmp("-sk", argv[1])) { type = SIG_KILL; }
	else { type = PG_SIG_TERM; }
	const pid_t pid = atoi(argv[2]);

	switch (type) {
	case PG_SIG_TERM:	igs::resource::killpg_sigterm(pid); break;
	case PG_SIG_INT:	igs::resource::killpg_sigint(pid);  break;
	case PG_SIG_KILL:	igs::resource::killpg_sigkill(pid); break;
	case SIG_TERM:	igs::resource::kill_sigterm(pid); break;
	case SIG_INT:	igs::resource::kill_sigint(pid);  break;
	case SIG_KILL:	igs::resource::kill_sigkill(pid); break;
	}
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_KILL_PROCESS */
/*
# gcc(32/64bits)  :199,200 w! tes_gcc_igs_rsc_kill_proc.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_KILL_PROCESS -I. igs_resource_kill_process.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_kill_proc
*/
