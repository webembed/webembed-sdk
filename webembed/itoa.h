#ifndef ITOA_H
#define ITOA_H
#ifdef __cplusplus
extern "C"{
#endif
void strreverse(char* begin, char* end);
void itoa(int value, char* str, int base);
#ifdef __cplusplus
}
#endif
#endif
