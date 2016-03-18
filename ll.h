#ifndef __CPUINFO_LL_H
#define __CPUINFO_LL_H

#if defined(_M_X64)
#define IDTR_LEN		10
#define GDTR_LEN		10
#define CR0_LEN			8
// #define LDTR_LEN		4
#elif defined(_M_IX86)
#define IDTR_LEN		6
#define GDTR_LEN		6
#define CR0_LEN			8
// #define LDTR_LEN		4
#endif

void ll_smsw(void *);
void ll_sgdt(void *);
/* void ll_sldt(void *); */

#endif /* __CPUINFO_LL_H */
