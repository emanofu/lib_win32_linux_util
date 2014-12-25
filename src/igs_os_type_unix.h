#ifndef igs_os_type_h
#define igs_os_type_h

/*------ �ӥåȿ������ѿ���� ----------------------------*/
# include <stdint.h> /* int32_t,uint32_t��C99��������줿��� */
typedef int32_t INT32_T;
typedef uint32_t UINT32_T;

/*------ 2bytesʸ���б� ----------------------------------*/
#if !defined TEXT
# ifdef     UNICODE
#  define TEXT(tt) L ## tt
typedef wchar_t WCHAR;
typedef WCHAR TCHAR;
# else  /* !UNICODE */
#  define TEXT(tt) tt
typedef char TCHAR;
# endif /* !UNICODE */
typedef const TCHAR* LPCTSTR;
typedef unsigned long long int DWORDLONG;
typedef UINT32_T DWORD;
#endif   /* !TEXT */

/*------ std::cout��Wide Character�б� -------------------*/
#ifndef   TCOUT
# ifdef UNICODE
#  define TCOUT wcout
# else
#  define TCOUT cout
# endif
#endif/* !TCOUT */

#endif /* !igs_os_type_h */
