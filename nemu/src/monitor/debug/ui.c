#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{"si", "si N : Let the program step by step after the implementation of N instructions suspended, When N is not given, the default is 1", cmd_si},
	{"info", "info r : Prints the register status, info w : Prints the monitoring point information", cmd_info},
	{"x", "X N EXPR: Calculate the value of the expression EXPR, the result as the starting memory address, in the form of hexadecimal output of the four N bytes", cmd_x},
	{"p", "p EXPR : calculate the value of the EXPR", cmd_p},
	{"w", "w EXPR : When the value of the expression EXPR changes, stop the program", cmd_w},
	{"d", "d N : Delete the monitoring point with the serial number N", cmd_d},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args){
	int n;
	if(args == NULL){
		cpu_exec(1);
		return 0;
	}
	else{
		if(sscanf(args, "%d", &n) == 1){
			cpu_exec(n);
			return 0;
		}
		else
			printf("UnKnown command '%s'\n", args);
	}
	return 0;
}

//void print_wp();
static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	if(arg == NULL){
		printf("lack parameters !!!\n");
		return 0;
	}
	if(strcmp(arg, "w") == 0){
		print_wp();	
		return 0;
	}
	else if(strcmp(arg, "r") == 0){
		int i = 0;
		for(i = 0; i < 8; i++){
			printf("%s\t 0x%x\t %d\t\n",regsl[i], reg_l(i), reg_l(i));
		}
		printf("eip\t 0x%x\t %d\t\n", cpu.eip, cpu.eip);
	}
	else
		printf("UnKnown command '%s'\n", arg);

	return 0;
}

static int cmd_x(char *args){
	int n;	
	char expression[32];
	if(args == NULL){
		printf("lack parameters !!!\n");
		return 0;
	}	
	else if(sscanf(args, "%d %s", &n, expression) == 2){	//此处有问题，expression不能含有空格，否则会被
		printf("args = %s\n", args);
		
		printf("n = %d, expression = %s\n", n, expression);
		int res = 0;
		bool flags = true;	
		res = expr(expression, &flags);
		if(flags == false){
			printf("error in function expr !!\n");
			return 0;
		}	
		while(n--){
			int i = 0;
			printf("0x");
		for(i = 0; i < 4; i++){
				printf("%02x",swaddr_read(res, 1));
				res += 1;
			}
			printf("\n");
		}
	}
	else
		printf("UnKnown command '%s'\n", args);

	return 0;
}

static int cmd_p(char *args){
	if(args == NULL){
		printf("lack parameters !!\n");
		return 0;
	}
	else{
		bool flags = true;
		int res = 0;
		res = expr(args, &flags);
		if(flags == false){
			printf("error in function expr !!\n");
		}
		else
			printf("result = %d\t0x%x\n", res, res);
	}
	return 0;
}

//void new_wp(char *args);
static int cmd_w(char *args){
	if(args == NULL){
		printf("lack parameters !!\n");
		return 0;
	}
	else{
		new_wp(args);	
	}
	return 0;
}

//void free_wp(int NO);
static int cmd_d(char *args){
	if(args == NULL){
		printf("lack parameters !!\n");
		return 0;
	}
	int n;
	if(sscanf(args, "%d", &n) == 1){
		free_wp(n);				
	}
	else
		printf("Unknown parameters '%s'\n", args);
	return 0;
}
	

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
