#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUM, NUM16, REG, NEQ, AND, OR, INVERT, DEREF, MINUS, LE, GE, SL, SR

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;	//优先级，值越大优先级越小
	int associate;	//结合性，左到右为1，右到左为0
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE, -1, 0},				// spaces
	{"\\+", '+', 4, 1},					// plus
	{"==", EQ, 7, 1},						// equal
	{"!=", NEQ, 7, 1},						// not equal
	{"-", '-', 4, 1},						// sub
	{"\\*", '*', 3, 1},					// mul
	{"/", '/', 3, 1},						// div
	{"%", '%', 3, 1},
	{"0x[0-9a-fA-F]+", NUM16, -1, 0},		//16进制数字,这一行一定要放到NUM上面！！！！！

	{"[0-9]+", NUM, -1, 0},				// number

	{"\\$[a-zA-Z]+", REG, -1, 0},				//寄存器
	{"\\(", '(', 1, 1},					//左括号
	{"\\)", ')', 1, 1},					//右括号
	{"&&", AND, 11, 1},				//逻辑与
	{"\\|\\|", OR, 12, 1},					//逻辑或
	{"\\!", INVERT, 2, 0},				//非
	{"\\*", DEREF, 2, 0},				//指针解引用
	{"-", MINUS, 2, 0},				//负号
	{"<=", LE, 6, 1},
	{">=", GE, 6, 1},
	{"<", '<', 6, 1},
	{">", '>', 6, 1},
	{"&", '&', 8, 1},
	{"\\|", '|', 10, 1},
	{"\\^", '^', 9, 1},			//按位异或
	{"<<", SL, 5, 1},				//左移
	{">>", SR, 5, 1},				//右移
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	int priority;
	int associate;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;
				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */
				if(substr_len > 31)
						Assert(0, "str is too long at position %d with len %d: %.*s\n", position, substr_len, substr_len, substr_start);
				else{
					tokens[nr_token].type = rules[i].token_type;
					tokens[nr_token].associate = rules[i].associate;
					tokens[nr_token].priority = rules[i].priority;
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = '\0';
					//printf("%s, %d\n", tokens[nr_token].str, tokens[nr_token].priority);
					
				}
				switch(rules[i].token_type) {
					case '+' :
					case '-' :
					case '*' :
					case '/' :
					case '%' :
					case '(' :
					case ')' :
					case NUM :
					case EQ  :
					case NEQ :
					case NUM16 :
					case REG :
					case AND :
					case OR :
					case INVERT :
					case LE :
					case GE :
					case '<' :
					case '>' :
					case '&' :
					case '|' :
					case '^' :
					case SL :
					case SR :
						break;
					case NOTYPE :
						--nr_token;
						break;	
					default: panic("please implement me");
				}
				++nr_token;	
				break;
			}
		}
		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p, int q){	//检查是否被一对括号包围
	if(tokens[p].type != '(' || tokens[q].type != ')')
		return false;	
	int num_left = 0, i = 0;
	for(i = p+1; i < q; i++){
		if(tokens[i].type == '(')
			num_left++;
		else if(tokens[i].type == ')')
			num_left--;
		if(num_left < 0)
			return false;	
	}
	if(num_left != 0)
		panic("expression is not valid !! \n");	
	return true;
}

int eval(int p, int q){
	if(p > q)
		panic("expression is not valid ?!\n");
	else if(p == q){
		if(tokens[p].type == NUM){
			int num = 0;
			char *tmp = tokens[p].str;
			while(*tmp != '\0'){
				num = num * 10 + *tmp - '0';
				tmp++;
			}
			return num;
		}
		else if(tokens[p].type == NUM16){
			int num = 0, x = 0;
			char *tmp = tokens[p].str + 2;
			while(*tmp != '\0'){
				if(*tmp >= '0' && *tmp <= '9')
					x = *tmp - '0';
				else if(*tmp >= 'a' && *tmp <= 'f')
					x = *tmp - 'a' + 10;
				else if(*tmp >= 'A' && *tmp <= 'F')
					x = *tmp - 'A' + 10;

				num = num * 16 + x;
				tmp++;
			}
			return num;
		}
		else if(tokens[p].type == REG){
			char *tmp = tokens[p].str + 1;
			if(strcmp(tmp, "eax") == 0 || strcmp(tmp, "EAX") == 0)
				return cpu.eax;
			else if(strcmp(tmp, "ebx") == 0 || strcmp(tmp, "EBX") == 0)
				return cpu.ebx;
			else if(strcmp(tmp, "ecx") == 0 || strcmp(tmp, "ECX") == 0)
				return cpu.ecx;
			else if(strcmp(tmp, "edx") == 0 || strcmp(tmp, "EDX") == 0)
				return cpu.edx;
			else if(strcmp(tmp, "esp") == 0 || strcmp(tmp, "ESP") == 0)
				return cpu.esp;
			else if(strcmp(tmp, "ebp") == 0 || strcmp(tmp, "EBP") == 0)
				return cpu.ebp;
			else if(strcmp(tmp, "esi") == 0 || strcmp(tmp, "ESI") == 0)
				return cpu.esi;
			else if(strcmp(tmp, "edi") == 0 || strcmp(tmp, "EDI") == 0)
				return cpu.edi;
			else if(strcmp(tmp, "eip") == 0 || strcmp(tmp, "EIP") == 0)
				return cpu.eip;
			else
				panic("Unknown register name %s !!\n", tokens[p].str);
			return 1;
		}
	}
	else if(check_parentheses(p, q))
		return eval(p+1, q-1);
	else{
		int i = 0, num_left = 0, op = p;
		for(i = p; i <= q; i++){
			//printf("str = %s, priority = %d\n", tokens[i].str, tokens[i].priority);
			if(tokens[i].type == '(')
				num_left++;			
			else if(tokens[i].type == ')')
				num_left--;
			else if(num_left == 0){
				//printf("%d, %d\n", tokens[i].priority, tokens[op].priority);
				if(tokens[i].priority >= tokens[op].priority && tokens[i].associate == 1){					
					op = i;
				}
			}
			if(num_left < 0)
				panic("parentheses do not match !! expression is not valid\n");
		}
		int val1 = 0, val2 = 0;
		if(tokens[op].type == DEREF || tokens[op].type == MINUS || tokens[op].type == INVERT)
			val1 = eval(op+1, q);
		else{
			val1 = eval(p, op-1);
			val2 = eval(op+1, q);
		}
		switch(tokens[op].type){
			case DEREF :
				return swaddr_read(val1, 4);
			case MINUS :
				return -val1;
			case INVERT :
				return !val1;
			case '+' : 
				return val1 + val2;
			case '-' :
				return val1 - val2;
			case '*' :
				return val1 * val2;
			case '/' :
				if(val2 == 0){
					printf("error, divide by zero !!\n");
					return 1;
				}
				else
					return val1 / val2;
			case '%' :
				if(val2 == 0){
					printf("error, divide by zero !!\n");
					return 1;
				}
				else
					return val1 % val2;

			case EQ :
				return val1 == val2;
			case NEQ :
				return val1 != val2;
			case AND :
				return val1 && val2;
			case OR :
				return val1 || val2;
			case LE :
				return val1 <= val2;
			case GE :
				return val1 >= val2;
			case '<' :
				return val1 < val2;
			case '>' :
				return val1 > val2;
			case '&' :
				return val1 & val2;
			case '|' :
				return val1 | val2;
			case '^' :
				return val1 ^ val2;
			case SL :
				return val1 << val2;
			case SR :
				return val1 >> val2;
			default : 
				assert(0);
			
		}
			
	}
	return 1;
}

uint32_t expr(char *e, bool *success) {
	if(strlen(e) >= 32){
		*success = false;
		printf("expression is too long\n");
		return 0;
	}
//	printf("args = %s\n", e);
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	int i = 0;
	for(i = 0; i < nr_token; i++){
		if(tokens[i].type == '*' &&(i == 0 || (tokens[i-1].type != ')' && tokens[i-1].type != NUM && tokens[i-1].type != REG))){
			tokens[i].type = DEREF;
			tokens[i].priority = 2;
			tokens[i].associate = 0;
		}
	}

	for(i = 0; i < nr_token; i++){
		if(tokens[i].type == '-' &&(i == 0 || (tokens[i-1].type != ')' && tokens[i-1].type != NUM && tokens[i-1].type != REG))){
			tokens[i].type = MINUS;
			tokens[i].priority = 2;
			tokens[i].associate = 0;
		}
	}

	//printf("nr_token = %d\n", nr_token);
	*success = true;
	return eval(0, nr_token - 1);
	//return 0;
}

