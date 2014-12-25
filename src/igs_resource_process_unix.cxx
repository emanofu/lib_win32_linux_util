#include <unistd.h>	// STDOUT_FILENO, STDERR_FILENO
#include <iomanip>	// std::setw()
#include <fcntl.h>	// ::fcntl(-)
#include <sys/wait.h>	// ::waitpid(-), WIFEXITED(-), WIFSIGNALED(-)
#include <cstring>	// ::memset(-)
#include <cerrno>	// errno
#include <iostream>	// std::cout, std::cerr, std::endl
#include <sstream>	// std::ostringstream
#include <string>	// std::string
#include <stdexcept>	// std::domain_error

#include "igs_resource_msg_from_err.h"
#include "igs_resource_process.h"
//--------------------------------------------------------------------
igs::resource::process::parameters::parameters()
	:cpid(0)
	,exit_code(-1)
	,end_char(0)
{
	this->fdout[0] = this->fdout[1] = 0;
	this->fderr[0] = this->fderr[1] = 0;
	this->fdend[0] = this->fdend[1] = 0;
}
igs::resource::process::parameters::~parameters() {
	try { this->close(); } catch(...) {}/* destructorから例外投げない */
}
const std::string igs::resource::process::parameters::close( void ) {
	std::string err_msg;
	if (0 == this->cpid) {
		return err_msg;
	}
	/*
		execv()したプロセスはwait()によって消滅させなければならない
		Wait for end of child process
		waitpid(0,-,-) : 呼び出しと同じプロセスグループに属する
				 子プロセスだけを待つ
	*/
	int status = 0;
	/* WNOHANG -> 子プロセスが存在しない場合にはすぐに返る */
	if (-1 == ::waitpid(this->cpid,&status,WNOHANG)) {
		this->cpid = 0;
		return igs_resource_msg_from_err("waitpid(-)",errno);
	}
	this->cpid = 0;
	/* 子プロセスが正常に終了していない時 */
	if (true != WIFEXITED(status)) {
		err_msg += "error exit about child process";
	}
	/* 孫プロセスが捕獲(catch)していない
	シグナルによって終了した場合 */
	if (WIFSIGNALED(status)) {
		std::ostringstream ost;
		ost << "error exit(";
		ost << WTERMSIG(status);
		ost << ") by signal about child process\n";
		err_msg += ost.str();
	}
	return err_msg;
}
//--------------------------------------------------------------------
#include <fstream>     // std::ofstream
namespace {
  void set_pipe_to_nonblock_(int fd) {
	const int flag = ::fcntl(fd,F_GETFL); if (flag < 0) {
		::perror("fcntl(,F_GETFL)");
	}
	if ((::fcntl(fd,F_SETFL,flag|O_NONBLOCK)) < 0) {
		::perror("fcntl(,F_SETFL,)");
	}
  }
  std::string em_(const int en,const char *msg) {
	std::string err_msg;
	err_msg += igs_resource_msg_from_err( msg ,en );
	err_msg += '\n';

	std::ofstream ofs;
	ofs.open("log_igs_resource_process_unix.txt"
		,std::ios::out|std::ios::app);
	ofs << err_msg;
	ofs.close();

	return err_msg;
  }
  std::string em_(const char *msg) {
	std::string err_msg;
	err_msg += msg;
	err_msg += '\n';

	std::ofstream ofs;
	ofs.open("log_igs_resource_process_unix.txt"
		,std::ios::out|std::ios::app);
	ofs << err_msg;
	ofs.close();

	return err_msg;
  }
}
void igs::resource::process::execute(
	igs::resource::process::parameters&pa
	,int argc
	,char *argv[]
) {
	if (::pipe(pa.fdout) < 0) {
		throw std::domain_error(igs_resource_msg_from_err(
			"pipe(fdout)",errno
		));
	}
	if (::pipe(pa.fderr) < 0) {
		throw std::domain_error(igs_resource_msg_from_err(
			"pipe(fderr)",errno
		));
	}
	if (::pipe(pa.fdend) < 0) {
		throw std::domain_error(igs_resource_msg_from_err(
			"pipe(fdend)",errno
		));
	}

	pa.cpid = ::fork(); /* 親プロセスから子プロセスを生成 */
	if (-1 == pa.cpid) {
		throw std::domain_error(igs_resource_msg_from_err(
			"fork()",errno
		));
	}

	if (0 != pa.cpid) { /* 親プロセス部分 */
		/* パイプのreadでブロックしないようにする */
		set_pipe_to_nonblock_(pa.fdout[0]);
		set_pipe_to_nonblock_(pa.fderr[0]);
		set_pipe_to_nonblock_(pa.fdend[0]);
		return;
	}
	else { /*  子プロセス部分 */
	  const pid_t ccpid = ::fork(); /* 子プロセスから孫プロセスを生成 */
	  if (-1 == ccpid) {
		em_(errno,"fork() in fork()");
		return;
	  }
	  if (0 != ccpid) { /*  孫に対する子プロセス部分 */
		/* 終了メッセージ('E')を親に伝えるためのパイプにつなぐ
			標準出力(STDOUT_FILENO)への出力が、
			pa.fdend[1]への出力となるように切り替える
		*/
		if (-1 == ::dup2(pa.fdend[1],STDOUT_FILENO)) {
		    em_(errno,"dup2(fdend[1])"); return; }
		if (0 != ::close(pa.fdend[0])) {
		   em_(errno,"close(fdend[0])"); return; }
		if (0 != ::close(pa.fdend[1])) {
		   em_(errno,"close(fdend[1])"); return; }

		/*
		execv()したプロセスはwait()によって消滅させなければならない
		Wait for end of process
		waitpid(0,-,-) : 呼び出しと同じプロセスグループに属する
				 子プロセスだけを待つ
		*/
		int status = 0;
		if (-1 == ::waitpid(0,&status,0)) {
			em_(errno,"waitpid(-)");
			return;
		}
		/* 孫プロセスが正常に終了していない時 */
		if (true != WIFEXITED(status)) {
			em_("error exit about grandson process");
		}
		/* 孫プロセスが捕獲(catch)していない
		シグナルによって終了した場合 */
		if (WIFSIGNALED(status)) {
			std::ostringstream ost;
			ost << "error exit(";
			ost << WTERMSIG(status);
			ost << ") by signal about grandson process";
			em_(ost.str().c_str());
		}
		std::string str;
		{
			std::ostringstream ost;
			ost << "E ";
			/* WEXITSTATUSは、
			終了した子プロセスの返り値の下位 8 ビットを返す
			WIFEXITEDが真を返した場合にのみ用いることができる */
			ost << WEXITSTATUS(status);
			str = ost.str(); // "E 0"
		}
		std::cout << str; std::cout.flush(); // end message
	  } else { /* 孫プロセス */
		/* メッセージを親に伝えるためのパイプにつなぐ
			標準出力(STDOUT_FILENO)への出力が、
			pa.fdout[1]への出力となるように切り替える
		*/
		if (-1 == ::dup2(pa.fdout[1],STDOUT_FILENO)) {
		    em_(errno,"dup2(fdout[1])"); return; }
		if (0 != ::close(pa.fdout[0])) {
		   em_(errno,"close(fdout[0])"); return; }
		if (0 != ::close(pa.fdout[1])) {
		   em_(errno,"close(fdout[1])"); return; }

		/* メッセージを親に伝えるためのパイプにつなぐ
			標準出力(STDERR_FILENO)への出力が、
			pa.fderr[1]への出力となるように切り替える
		*/
		if (-1 == ::dup2(pa.fderr[1],STDERR_FILENO)) {
		    em_(errno,"dup2(fderr[1])"); return; }
		if (0 != ::close(pa.fderr[0])) {
		   em_(errno,"close(fderr[0])"); return; }
		if (0 != ::close(pa.fderr[1])) {
		   em_(errno,"close(fderr[1])"); return; }
		/*
		実行プログラムの子どもプロセスもkillしたい場合どうする???
		*/
		::execv(argv[0],argv);
		//::perror(argv[0]);
		//exit(1);
	  }
	}
}

namespace {
  template <class type>
  const std::string form_(
	const char* title
	,const char* key
	,const type val
	,const int key_wide=9
  ) {
	std::ostringstream ost;
	ost
		<< title
		<< ' ' << std::setw(key_wide) << std::left << key
		<< ' ' << val;
	return ost.str();
  }
}
void igs::resource::process::form(
	const igs::resource::process::parameters&pa
	,std::vector< std::string >&lines
) {
  lines.push_back(form_("pipe","fdout0",pa.fdout[0]));
  lines.push_back(form_("pipe","fdout1",pa.fdout[1]));
  lines.push_back(form_("pipe","fderr0",pa.fderr[0]));
  lines.push_back(form_("pipe","fderr1",pa.fderr[1]));
  lines.push_back(form_("pipe","fdend0",pa.fdend[0]));
  lines.push_back(form_("pipe","fdend1",pa.fdend[1]));
  lines.push_back(form_("proc","cpid"  ,pa.cpid));
}

namespace {
  void debug_print_(const char *title,const int len,const char *buf) {
	std::cout << title << " len=" << len << " buf=<" << buf << ">\n";
  }
}
bool igs::resource::process::wait(
	igs::resource::process::parameters&pa
	,const int sleep_sec
	,const int sleep_nsec /* 999,999,999以下 */
	,std::string&out_str
	,std::string&err_str
) {
	/* 指定時間待ち、その間プロセスの終了は感知しないことに注意 */
	struct timespec req,rem;
	req.tv_sec  = sleep_sec;
	req.tv_nsec = sleep_nsec;
	::nanosleep(&req,&rem);

	/* 子プロセスの終了感知 */
	char end_buf[4096] = {}; ::memset(end_buf,0,sizeof(end_buf));
	const int end_len = ::read(pa.fdend[0],end_buf,sizeof(end_buf)-1);
	if (0 < end_len) {
		pa.end_char = end_buf[0];
		if (3 <= strlen(end_buf)) {
			pa.exit_code = atoi(&end_buf[2]);
		}
	}
#if defined DEBUG_IGS_RESOURCE_PROCESS
  debug_print_("end",end_len,end_buf);
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */

	/* 子プロセスのstdoutを受け取り */
	char out_buf[4096] = {}; ::memset(out_buf,0,sizeof(out_buf));
	const int out_len = ::read(pa.fdout[0],out_buf,sizeof(out_buf)-1);
	if (0 < out_len) { out_str += out_buf; }
#if defined DEBUG_IGS_RESOURCE_PROCESS
  debug_print_("out",out_len,out_buf);
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */

	/* 子プロセスのstderrを受け取り */
	char err_buf[4096] = {}; ::memset(err_buf,0,sizeof(err_buf));
	const int err_len = ::read(pa.fderr[0],err_buf,sizeof(err_buf)-1);
	if (0 < err_len) { err_str += err_buf; }
#if defined DEBUG_IGS_RESOURCE_PROCESS
  debug_print_("err",err_len,err_buf);
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */

	return (0 <= out_len) || (0 <= err_len) || (pa.end_char != 'E');
}
pid_t igs::resource::process::id(
	const igs::resource::process::parameters&pa
) {
	return pa.cpid;
}
int igs::resource::process::exit_code(
	const igs::resource::process::parameters&pa
) {
	return pa.exit_code;
}
void igs::resource::process::close(
	igs::resource::process::parameters&pa
) {
	std::string str( pa.close() );
	if (!str.empty()) {
		throw std::domain_error(igs_resource_msg_from_er( str ));
	}
}
#if defined DEBUG_IGS_RESOURCE_PROCESS
# include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[])
{
	if (1 == argc) {
		std::cout << "Usage : " << argv[0] << " commandline ...\n";
		return 1;
	}
	int ret = 0;
  try {
	igs::resource::process::parameters pa;
	igs::resource::process::execute(pa,argc-1,&argv[1]);
	std::cout << "pid=" << igs::resource::process::id(pa) << std::endl;

#if defined _WIN32 && defined UNICODE
	std::vector< std::basic_string<TCHAR> > lines;
	igs::resource::process::form( pa,lines );
	for (unsigned ii=0;ii<lines.size(); ++ii) {
		std::wcout << lines.at(ii) << std::endl;
	}
#else
	std::vector< std::string > lines;
	igs::resource::process::form( pa,lines );
	for (unsigned ii=0;ii<lines.size(); ++ii) {
		std::cout << lines.at(ii) << std::endl;
	}
#endif

	std::string sout,serr,msg;
	while (igs::resource::process::wait(pa,1,0,sout,serr)) {
		/* jobからのメッセージを処理する */
		if (!sout.empty()){
			std::cout << "out_now " << sout;
			msg += "out_now "; msg += sout;
			sout.clear();
		}
		if (!serr.empty()){
			std::cout << "err_now " << serr;
			msg += "err_now "; msg += serr;
			serr.clear();
		}
	}
	std::cout
		<< "------------\n"
		<< msg
		<< "------------\n"
		;
	ret = igs::resource::process::exit_code(pa);
	std::cout << "exit_code=" << ret << std::endl;
	igs::resource::process::close(pa);
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_PROCESS */
/*
# gcc(32/64bits) :367,368 w! tes_gcc_igs_rsc_process.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_PROCESS -I. igs_resource_process.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_process
// :327,347 w! tes_gcc_process.cxx
// g++ tes_gcc_process.cxx -o tes_gcc_process
#include <iostream>
int main(int argc,char *argv[]) {
	unsigned loop=50;
	int seco=1;
	if (1 != argc && 3 != argc) {
		std::cout << "Usage : " << argv[0] << " loop second\n";
		return 1;
	}
	if (3 == argc) {
		loop = atoi(argv[1]);
		seco = atoi(argv[2]);
	}
	for (unsigned ii=0;ii<loop;++ii) {
		std::cout << "message about cout " << ii << "time\n";
		std::cerr << "message about cerr " << ii << "time\n";
		::sleep(seco);
	}
	return 0;
}
*/
