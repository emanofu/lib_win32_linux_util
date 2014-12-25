#include <fstream>
#include <string>
#include "igs_resource_number_of_processors.h"
unsigned int igs::resource::number_of_processors(void) {
	std::ifstream ifs("/proc/cpuinfo");
	std::string str;
	unsigned int count = 0;
	while (!ifs.eof()) {
		ifs >> str;
		if ("processor" == str) { ++count; }
	}
	return count;
}

#if defined DEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS
#include <iostream>
#include <sys/sysinfo.h> // struct sysinfo s_info;
int main(void) {
  try {
	std::cout << "number_of_processors="
		<< igs::resource::number_of_processors()
		<< std::endl;
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS */
/*
# gcc(32/64bits) :30,32 w! tes_gcc_igs_rsc_n_of_p.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS -I. igs_resource_number_of_processors.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_n_of_p_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_NUMBER_OF_PROCESSORS -DUNICODE -I. igs_resource_number_of_processors.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_n_of_p_wc
*/
