//Miscellaneous function prototypes missing elsewhere, mostly from espmissingincludes.h
#ifndef ESPMISC_H

#define ESPMISC_H

#ifdef __cplusplus
extern "C"{
#endif
char *ets_strstr(const char *haystack, const char *needle);
int ets_strncmp(const char *s1, const char *s2, int len);
//Needed to enable use of os_printf
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

#ifdef __cplusplus
}
#endif
#endif
