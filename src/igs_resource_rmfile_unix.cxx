/*
�ե�����1�ĺ��
�ե����뤬�ʤ�������Ǥ��ʤ����Ȥ������顼�㳰���ꤲ��
*/
# include <unistd.h> // unlink(-)
# include <cerrno>
# include <stdexcept> // std::domain_error
# include "igs_resource_msg_from_err.h"
# include "igs_resource_rmfile.h"
void igs::resource::rmfile(LPCTSTR file_path) {
/*
Linux 2.6.7(2004-06-23)unlink(-)�Х�
NFS �ץ�ȥ��� (protocol) ������Ū�����ɤˤ�ꡢ�ޤ�������Υե����� ��
�����ξ��Ǥ�������������Ȥ����롣
*/
	/* unlink()�����������0���֤������Ԥ����-1���֤� */
	if (0 != ::unlink(igs::resource::mbs_from_ts(file_path).c_str())) {
		throw std::domain_error(igs_resource_msg_from_err(
			file_path,errno
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_RMFILE
# include <iostream>
# include "igs_resource_msg_from_err.cxx"
int main( int argc, char *argv[] )
{
	if (2 != argc) {
		std::cout << "Usage " << argv[0] << " file_path\n";
		return 1;
	}
  try {
	igs::resource::rmfile(igs::resource::ts_from_mbs(argv[1]).c_str());
  }
  catch (std::exception& e) {std::cerr<<"exception="<<e.what()<<std::endl;}
  catch (...) { std::cerr << "other exception" << std::endl; }
	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_RMFILE */
/*
# gcc(32/64bits) :47,49 w! tes_gcc_igs_rsc_rmfile.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_RMFILE -I. igs_resource_rmfile.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_rmfile_mb
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_RMFILE -DUNICODE -I. igs_resource_rmfile.cxx -o tes_gcc_${HOSTTYPE}_igs_rsc_rmfile_wc
*/
