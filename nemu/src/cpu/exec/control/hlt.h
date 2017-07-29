#ifndef __HLT_H__
#define __HLT_H__

make_helper(hlt){
	while(!cpu.INTR)
		;
	print_asm("hlt");
	return 1;
}


#endif

