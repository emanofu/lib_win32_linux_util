#include <cstdlib> // ::strtoull()
#include <cerrno> // cerrno
#include <stdexcept> // std::domain_error()
#include <fstream> // std::ifstream
#include <sstream> // std::istringstream
#include "igs_resource_msg_from_err.h"
#include "igs_resource_memory_status.h"
void igs::resource::memory_status(
	unsigned long long int&commit_charge_total_kbytes
	, unsigned long long int&commit_charge_limit_kbytes
	, unsigned long long int&total_physical_kbytes
) {
	const char *file_path = "/proc/meminfo";
	std::ifstream ifs(file_path);
	if (!ifs) {
		std::string str(file_path);
		str += ":can not ifs.open(-)";
		throw std::domain_error(igs_resource_msg_from_er(
			igs::resource::ts_from_mbs(str)
		));
	}
	std::string line;

	while (std::getline(ifs,line)) {
		if (ifs.bad()) { /* 致命的なエラー */
		 std::string str(file_path);
		 str += ":ifs.bad(-) is not zero";
		 throw std::domain_error(igs_resource_msg_from_er(
			igs::resource::ts_from_mbs(str)
		 ));
		}
		if (ifs.fail()) { continue; } /* 致命的ではないエラー */
		if (line.size() <= 0) { continue; } /* 空行 */

		std::istringstream ist(line);
		std::string key,val;
		ist >> key >> val;
		
		if      ("MemTotal:" == key) {
			errno = 0;
			total_physical_kbytes = ::strtoull(val.c_str(),0,0);
			if (0 != errno) {
			 throw std::domain_error( igs_resource_msg_from_err(
				TEXT("MemTotal in /proc/meminfo") ,errno
			 ));
			}
		}
		else if ("MemFree:"  == key) {
			errno = 0;
			commit_charge_total_kbytes = ::strtoull(
							val.c_str(),0,0);
			if (0 != errno) {
			 throw std::domain_error( igs_resource_msg_from_err(
				TEXT("MemFree in /proc/meminfo") ,errno
			 ));
			}
			commit_charge_total_kbytes = (
				total_physical_kbytes -
				commit_charge_total_kbytes
			);
		}
		else if ("SwapTotal:" == key) {
			errno = 0;
			commit_charge_limit_kbytes = total_physical_kbytes +
					::strtoull( val.c_str(),0,0);
			if (0 != errno) {
			 throw std::domain_error( igs_resource_msg_from_err(
				TEXT("SwapTotal in /proc/meminfo") ,errno
			 ));
			}
			/* "SwapTotal:"が最後と仮定して終了する */
			break;
		}
	}
	ifs.close();
}

#if defined DEBUG_IGS_RESOURCE_MEMORY_STATUS
# include <iostream>
# include <iomanip>
# include <sstream>
# include <sys/sysinfo.h> // struct sysinfo s_info;
# include "igs_resource_msg_from_err.cxx"
int main(int argc, char *argv[])
{
  try {
	struct sysinfo s_info; ::memset(&s_info,0,sizeof(s_info));
	const int error = ::sysinfo(&s_info);
	std::cout
	<< "::sysinfo()=" << error
	<< "\nTotal usable main memory size(totalram)\t=" << s_info.totalram
	<< "\nAvailable memory size(freeram)\t=" << s_info.freeram
	<< "\nAmount of shared memory(sharedram)\t=" << s_info.sharedram
	<< "\nMemory used by buffers(bufferram)\t=" << s_info.bufferram
	<< "\nTotal swap space size(totalswap)\t=" << s_info.totalswap
	<< "\nswap space still available(freeswap)\t=" << s_info.freeswap
	<< "\nNumber of current processes(procs)\t=" << s_info.procs
	<< "\nTotal high memory size(totalhigh)\t=" << s_info.totalhigh 
	<< "\nAvailable high memory size(freehigh)\t=" << s_info.freehigh  
	<< "\nMemory unit size in bytes(mem_unit)\t=" << s_info.mem_unit   
	<< std::endl
	<< std::endl
	;

	unsigned long long int commit_charge_total_kbytes=0;
	unsigned long long int commit_charge_limit_kbytes=0;
	unsigned long long int total_physical_kbytes=0;
	igs::resource::memory_status(
		commit_charge_total_kbytes
		,commit_charge_limit_kbytes
		,total_physical_kbytes
	);
	std::cout
		<< "Commit Charge Total\t"
		<< std::setw(13)
		<< commit_charge_total_kbytes << "Kbytes"

		<< std::setw(9)
		<< commit_charge_total_kbytes/1024 << "MiB"

		<< std::setw(5)
		<< commit_charge_total_kbytes/1024/1024 << "GiB\n"

		<< "Commit Charge Limit\t"
		<< std::setw(13)
		<< commit_charge_limit_kbytes << "Kbytes"

		<< std::setw(9)
		<< commit_charge_limit_kbytes/1024 << "MiB"

		<< std::setw(5)
		<< commit_charge_limit_kbytes/1024/1024 << "GiB\n"

		<< "Total Physical\t\t"
		<< std::setw(13)
		<< total_physical_kbytes << "Kbytes"

		<< std::setw(9)
		<< total_physical_kbytes/1024 << "MiB"

		<< std::setw(5)
		<< total_physical_kbytes/1024/1024 << "GiB\n"

		<< "Commit Charge / Total Physical\t"
		<< std::setw(3)
		<< 100*commit_charge_total_kbytes/total_physical_kbytes
			<< "%"

		<< std::endl
		;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_MEMORY_STATUS */
/*
# gcc(32/64bits)  :158,160 w! tes_gcc_igs_rsc_memory.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_MEMORY_STATUS -I. igs_resource_memory_status.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_memory_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_MEMORY_STATUS -DUNICODE -I. igs_resource_memory_status.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_memory_wc
*/
