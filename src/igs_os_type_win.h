#ifndef igs_os_type_h
#define igs_os_type_h

/*------ 2bytesʸ���б� ----------------------------------*/
# include <windows.h> /* INT32,UINT32 */

/*------ �ӥåȿ������ѿ���� ----------------------------*/
typedef INT32 INT32_T;
typedef UINT32 UINT32_T;

/*------ std::cout��Wide Character�б� -------------------*/
#ifndef   TCOUT
# ifdef UNICODE
#  define TCOUT wcout
# else
#  define TCOUT cout
# endif
#endif/* !TCOUT */

#endif /* !igs_os_type_h */
