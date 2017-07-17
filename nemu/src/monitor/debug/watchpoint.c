#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "monitor/monitor.h"
#include "nemu.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;	//head 用于组织使用中的监视点结构, free_ 用于组织空闲的监视点结构

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

/*new_wp() 从 free_ 链表中返回一个空闲的监视点结构*/
void new_wp(char *args){
	if(free_ == NULL)
		assert(0);
	WP *tmp = free_;
	strcpy(tmp->expr, args);
	bool flags = true;
	tmp->lastVal = expr(args, &flags);
	if(flags == false){
		printf("Create Watchpoints Falied because of invalid expr!!\n");
		return;
	}
	free_ = free_->next;

	if(head == NULL){
		head = tmp;
		tmp->next = NULL;
	}
	else{
		WP *p = head;
		while(p->next)
			p = p->next;
		p->next = tmp;
		tmp->next = NULL;
	}	
}

/*free_wp() 将 wp 归还到 free_ 链表中*/
void free_wp(int NO){
	WP *wp = head;
	while(wp){
		if(wp->NO == NO)
			break;
		wp = wp->next;
	}
	if(wp == NULL){
		printf("There is no Watchpoints %d\n", NO);
		return;
	}
	
	if(wp == head)
		head = head->next;
	else{
		WP *p = head;
		while(p->next != wp)
			p = p->next;

		p->next = p->next->next;	
	}
	wp->next = NULL;
	printf("Delete watchpoints %d Successful !!\n", NO);
	if(free_ == NULL)
		free_ = wp;
	else{
		WP *p = free_;
		while(p->next)
			p = p->next;
		p->next = wp;
		wp->next = NULL;
	}
}

void check_wp(int *nemu_state){
	if(head != NULL){
		WP *p = head;
		while(p){
			bool flags = true;
			int current = expr(p->expr, &flags);
			if(current != p->lastVal){
				*nemu_state = STOP;
				printf("0x%x hit watchpoints %d\n", cpu.eip, p->NO);
				printf("old_val = %d, 0x%x\n", p->lastVal, p->lastVal);
				printf("new_val = %d, 0x%x\n", current, current);
				p->lastVal = current;
			}
			p = p->next;
		}
	}
				
}

void print_wp(){
	if(head == NULL){
		printf("No Watchpoints !!\n");
		return;
	}
	printf("Num\t\tWhat\t\tValue\n");
	WP *p = head;
	while(p){
		printf("%d\t\t%s\t\t%d\n", p->NO, p->expr, p->lastVal);
		p = p->next;
	}


}

