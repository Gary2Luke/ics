#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	int lastVal;		//表达式上次求值的结果
	char expr[32];		//监视的表达式
	struct watchpoint *next;

	/* TODO: Add more members if necessary */


} WP;

void new_wp(char*);
void check_wp(int *nemu_state);
void free_wp(int NO);
void print_wp();
#endif
