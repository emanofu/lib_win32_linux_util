#include <stdexcept>	// std::domain_error()
#include "igs_resource_msg_from_err.h"
#include "igs_resource_thread.h"

//--------------------------------------------------------------------
// pthread_t = unsigned long int(rhel4)
/*
	state が
	PTHREAD_CREATE_JOINABLE なら、pthread_join()を呼ぶこと。
	PTHREAD_CREATE_DETACHED なら、なにも呼ぶ必要がないが、
		thread終了を知るには自前で仕掛けが必要。
*/
/*
joinしない場合は、functionの中で、
pthread_detach(pthread_self())を実行し、
で切り離す(ゾンビにはならない)
スレッドの終了時にリソースを切り離す
*/
pthread_t igs::resource::thread_run(
	void *(*function)(void *)
	, void *func_arg
	, const int state // PTHREAD_CREATE_JOINABLE/PTHREAD_CREATE_DETACHED
) {
	pthread_attr_t attr;
	if (::pthread_attr_init(&attr)) {
		throw std::domain_error(igs_resource_msg_from_er(
			"pthread_attr_init(-)"));
	}
	if (::pthread_attr_setdetachstate(&attr,state)) {
		throw std::domain_error(igs_resource_msg_from_er(
			"pthread_attr_setdetachstate(-)"));
	}

	pthread_t thread_id = 0;
	const int erno = ::pthread_create(
		&(thread_id),&attr,function,func_arg
	);

	if (0 != erno) {
		throw std::domain_error(igs_resource_msg_from_err(
			"pthread_create(-)",erno
		));
	}
	return thread_id;
}
/*
const bool igs::resource::thread_was_done(const pthread_t thread_id) {
??????????????????????????????????????????????????????????????????
??????????????????????????????????????????????????????????????????
??????????????????????????????????????????????????????????????????
threadの終了方法を見る関数は見つからない。
実行関数の引数で、終了フラグを立てて、外から感知する方法か。
関数が終了するまでの間のタイムラグがあるが、問題はあるのか???
}
*/
void igs::resource::thread_join(const pthread_t thread_id) {
	const int erno = ::pthread_join( thread_id, NULL );
	if (0 != erno) {
		throw std::domain_error(igs_resource_msg_from_err(
			"pthread_join(-)",erno
		));
	}
}
#if defined DEBUG_IGS_RESOURCE_THREAD
# include <iostream>
# include <vector>
# include <time.h>	// ::nanosleep()
# include "igs_resource_sleep.h"
# include "igs_resource_sleep.cxx"
# include "igs_resource_msg_from_err.cxx"
namespace {
 struct params {
	int param01;
	bool is_thread_end;
 };
 void *function1(void *arg) {
	params *par = static_cast<params *>(arg);
	std::cout << "thread begin<" << par->param01 << ">" << std::endl;

	//::usleep(3000000);
	igs::resource::sleep_sn(par->param01, 0); /* 秒、nano秒 */

	std::cout << "thread end<" << par->param01 << ">" << std::endl;
	par->is_thread_end = true;
	return 0;
 }
}
int main(int argc, char *argv[])
{
	std::vector<pthread_t> id(3);
	params arg1, arg2, arg3;
	arg1.param01 = 1; /* 秒 */
	arg2.param01 = 2; /* 秒 */
	arg3.param01 = 3; /* 秒 */

	std::cout << "--- thread for speed up ---" << std::endl;

	arg1.is_thread_end = false;
	arg2.is_thread_end = false;
	arg3.is_thread_end = false;
	id[0] = igs::resource::thread_run(
		function1,&arg1,PTHREAD_CREATE_JOINABLE
	);
	id[1] = igs::resource::thread_run(
		function1,&arg2,PTHREAD_CREATE_JOINABLE
	);
	id[2] = igs::resource::thread_run(
		function1,&arg3,PTHREAD_CREATE_JOINABLE
	);

	igs::resource::thread_join( id[0] );
	std::cout <<   "after join1" << std::endl;

	igs::resource::thread_join( id[2] );
	std::cout <<   "after join3" << std::endl;

	igs::resource::thread_join( id[1] );
	std::cout <<   "after join2" << std::endl;

	std::cout << "--- thread for jobs control ---" << std::endl;

	arg1.is_thread_end = false;
	arg2.is_thread_end = false;
	arg3.is_thread_end = false;
	id[2] = igs::resource::thread_run(
		function1,&arg3,PTHREAD_CREATE_DETACHED
	);
	igs::resource::sleep_sn(0,100000000); // 0.1sec
	id[1] = igs::resource::thread_run(
		function1,&arg2,PTHREAD_CREATE_DETACHED
	);
	igs::resource::sleep_sn(0,100000000); // 0.1sec
	id[0] = igs::resource::thread_run(
		function1,&arg1,PTHREAD_CREATE_DETACHED
	);
	igs::resource::sleep_sn(0,100000000); // 0.1sec
	std::cout << "wait..." << std::endl;
	for (int ii = 0;; ++ii) {
		/*
このループ内に(sleep()等)他のスレッドに処理をさせる余裕をもたせないと、
他のスレッドが終了しないのかもしれず、結果的にここが終了しない。
		*/
		//igs::resource::sleep_sn(0,1);
		igs::resource::sleep_sn(0,100000000);	// 0.1sec
		// std::cout << "\r" << ii;
		//std::cout << arg1.is_thread_end << arg2.is_thread_end << arg3.is_thread_end << " ";
		if (arg1.is_thread_end
		&&  arg2.is_thread_end
		&&  arg3.is_thread_end) {break;}
	}

	std::cout << "--- end ---" << std::endl;

	id.clear();

	return 0;
}
#endif  /* !DEBUG_IGS_RESOURCE_THREAD */
/*
# gcc(32/64bits) :148,149 w! tes_gcc_igs_rsc_thread.csh
g++ -Wall -O2 -g -DDEBUG_IGS_RESOURCE_THREAD -I. igs_resource_thread.cxx -lpthread -o tes_gcc_${HOSTTYPE}_igs_rsc_thread
*/
