//Miscellaneous function prototypes missing elsewhere, mostly from espmissingincludes.h
#ifndef ESPMISC_H

#define ESPMISC_H

#ifdef __cplusplus
extern "C"{
#endif
char *ets_strstr(const char *haystack, const char *needle);
int ets_strncmp(const char *s1, const char *s2, int len);
int ets_strcmp(const char *s1, const char *s2);
int ets_strlen(const char *s);
void *ets_memcpy(void *dest, const void *src, size_t n);
//Needed to enable use of os_printf
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));
int ets_sprintf(char *buf, const char *format, ...);

int os_snprintf(char *str, size_t size, const char *format, ...) __attribute__ ((format (printf, 3, 4)));
#ifdef __cplusplus
}
#endif
#endif
