#include <vector>	// std::vector
#include <fstream>	// std::ifstream
#include <sstream>	// std::istringstream
#include <stdexcept>	// std::domain_error()
#include <numeric>	// std::accumlate()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_sleep.h"
#include "igs_resource_cpu_rate.h"
namespace {
 /* cpuのプロセスステータス値を得る */
 void cpu_process_status_(std::vector<int>&ints) {
	/* ファイルを開く */
	const char *file_path = "/proc/stat";
	std::ifstream ifs;
	ifs.open(file_path);
	if (!ifs) {
		std::string str(file_path); str+= " can not ifs.open()";
		throw std::domain_error(igs_resource_msg_from_er(str));
	}

	/* 一行だけ読み込む */
	std::string line;
	if (!std::getline(ifs,line)) {
		std::string str(file_path); str+= " std::getline() is zero";
		throw std::domain_error(igs_resource_msg_from_er(str));
	}

	/* 読込みの致命的なエラー */
	if (ifs.bad()) {
		std::string str(file_path); str+= ":ifs.bad() is not zero";
		throw std::domain_error(igs_resource_msg_from_er(str));
	}

	/* ファイルを閉じる */
	ifs.close();

	/* 一行を項目に分ける */
	std::istringstream ist(line);
	std::vector< std::string > words;
	for (unsigned ii=0;;++ii) {
		std::string item;
		ist >> item;
		if (item.empty()) {break;}
		words.push_back(item);
	}

	/* 項目チェック */
	if (words.size() <= 0 || "cpu" != words.at(0)) {
		std::string str(file_path); str+=" do not has keyword(cpu)";
		throw std::domain_error(igs_resource_msg_from_er(str));
	}

	/* 項目を数値に変換する */
	for (unsigned ii=1; ii<words.size(); ++ii) {
		int val=0;
		std::istringstream ist( words.at(ii) ); ist >> val;
		ints.push_back(val);
	}
 }
}

double igs::resource::cpu_rate(
	const time_t seconds
	,const long nano_seconds
) {
	std::vector<int> ints1,ints2,ints3;
	cpu_process_status_(ints1);
	igs::resource::sleep_sn(seconds, nano_seconds);
	cpu_process_status_(ints2);

	/* プロセス時間の差を取る */
	for (unsigned ii=0; ii<ints1.size() && ii<ints2.size(); ++ii) {
		ints3.push_back( ints2.at(ii) - ints1.at(ii) );
	}

	/* 全体のプロセス時間を計算 */
	int total = std::accumulate( ints3.begin(), ints3.end(), 0 );

	/* CPUの使用比率を0...1の値で返す */
	if (ints3.size() <= 3) { return 0; }
	return static_cast<double>(total - ints3.at(3)) / total;
}
//--------------------------------------------------------------------
#if defined DEBUG_IGS_RESOURCE_CPU_RATE
# include <iostream>	// std::cout
# include <sys/sysinfo.h> // struct sysinfo s_info;
# include "igs_resource_msg_from_err.cxx"
# include "igs_resource_sleep.cxx"
int main(int argc,char *argv[]) {
  try {
	if (3 != argc) {
		std::cout << "Usage : " << argv[0]
			<< " seconds nano_seconds"
			<< std::endl;
		return 1;
	}
	std::cout
		<< "cpu rate(unix)="
		<< igs::resource::cpu_rate(
			atol(argv[1]), atol(argv[2]) )
		<< std::endl;
	struct sysinfo s_info;
	const int error = ::sysinfo(&s_info);
	std::cout
		<< "by sysinfo()\n"
		<< " ret=" << error
		<< " Uptime=" << s_info.uptime
		<< " Load: 1min=" << s_info.loads[0]
		<< " 5min=" << s_info.loads[1]
		<< " 15min=" << s_info.loads[2]
		<< std::endl;

  }
  catch (std::exception& e) {std::cerr<<"exception=\""<<e.what()<<"\"\n";}
  catch (...)               {std::cerr<<"other exception"<<std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_CPU_RATE */
/*
# gcc(32/64bits) :120,121 w! tes_gcc_igs_rsc_cpu_rate.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_CPU_RATE -I. igs_resource_cpu_rate.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_cpu_rate
*/
