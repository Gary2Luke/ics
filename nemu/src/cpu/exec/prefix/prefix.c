#include "cpu/exec/helper.h"

make_helper(exec);

make_helper(operand_size) {						//处理0x66前缀 operand_size prefix
	ops_decoded.is_operand_size_16 = true;		//操作数大小为16位
	int instr_len = exec(eip + 1);
	ops_decoded.is_operand_size_16 = false;			//执行完毕，恢复全局变量ops_decoded默认操作数大小为32位
	return instr_len + 1;
}
