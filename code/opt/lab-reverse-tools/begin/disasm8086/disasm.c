#include "disasm.h"

int main(int argc, char **argv)
{
	char *buffer; 
	long num ; 
	if (argc > 1)
	{
		if (strlen(argv[1]) < 255)
		{
			buffer = read_file(argv[1], &num) ;
			disasm(buffer, num) ;
		}
	}
	return 0 ;
}


char *regs16[8] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"} ; 
char *regs8[8] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"} ;
char *segreg[4] = {"es", "cs", "ss", "ds"} ;

enum segment_registers {ES=0, CS, SS, DS} ; 

signed char segment_override = -1 ;
 
int bytes = 0 ;
int num_bytes = 0 ;

int rm_segment_override = -1 ; 

int parse(char *s, char*(*func)(char*, int*, int *), char *buffer, int *j)
{
	bytes = 1;
	int temp_j = *j ; 
	int error = 0 ;
	char *result = func(buffer, j, &error) ;
	if (error)
	{
		char tmp_buffer[20] ; 
		memset(tmp_buffer, '\0', 20) ;
		unsigned char tmp_char = buffer[*j] ; 
		sprintf(tmp_buffer, "db 0x%X\n", tmp_char) ;
		parse_noop(tmp_buffer, buffer, j) ;
		return 0 ;
	}
	int i=0;
	int k = 16 ;
	int t = 0 ;
	if (segment_override == -1 && rm_segment_override >= 0)
	{
		t = 1; 
		k = k - 2 ;
		switch (rm_segment_override)
		{
			case ES: printf("%02X", 0x26) ;	 break ;
			case CS: printf("%02X", 0x2E) ;	 break ;
			case SS: printf("%02X", 0x36) ;	 break ;
			case DS: printf("%02X", 0x3E) ;	 break ;
		} 
		rm_segment_override = -1 ; 
		segment_override = -1 ;
	}
	char segment[20] ;

	if (t == 1)
	{
			memset(segment, '\0', 20) ;
			switch (segment_override)
			{
				case ES: sprintf(segment, "es") ; break ;
				case CS: sprintf(segment, "cs") ; break ;
				case SS: sprintf(segment, "ss") ; break ;
				case DS: sprintf(segment, "ds") ; break ;
			}
	}
	if (segment_override >= 0 )
	{
		k = k - 2 ;
		switch (segment_override)
		{
			case ES: printf("%02X", 0x26) ;	 break ;
			case CS: printf("%02X", 0x2E) ;	 break ;
			case SS: printf("%02X", 0x36) ;	 break ;
			case DS: printf("%02X", 0x3E) ;	 break ;
		}
		segment_override = -1 ;
		rm_segment_override = -1 ; 
	}
	for (i=0; i < bytes; i++)
	{
		unsigned char byte = buffer[temp_j+i]  ; 
		printf("%02X", byte) ;	
	}
	k = (k - (bytes*2))  ; 
	for (i=0; i < k; i++) printf(" ") ;
	if (t == 1)
	{
		char tmp_string[255] ; 
		char tmp_string2[255] ; 
		memset(tmp_string, '\0', 255) ;
		memset(tmp_string2, '\0', 255) ;
		sprintf(tmp_string, s, result) ; 
		sprintf(tmp_string2, "%s %s", segment, tmp_string) ; 
		printf("%s", tmp_string2) ;
	}
	else printf(s, result) ; 
}

int get_bytes(int k, int j)
{
	if (k + j < num_bytes) return 0; 
	else return 1; 
}

int parse_noop(char *s, char *buffer, int *j)
{
	int k = 16 ; 
	if (segment_override >= 0) 
	{
			switch (segment_override)
			{
				case ES: printf("%02X", 0x26) ;	 break ;
				case CS: printf("%02X", 0x2E) ;	 break ;
				case SS: printf("%02X", 0x36) ;	 break ;
				case DS: printf("%02X", 0x3E) ;	 break ;
			}
			k = k - 2 ; 
	}
	unsigned char tmp_char = buffer[*j] ; 
	printf("%02X", tmp_char) ;	
	int i= 0 ;
	k = k - 1*2 ; 
	for (i=0; i < k; i++) printf(" ") ;
	if (segment_override >= 0)
	{
		char segment[20] ;
		memset(segment, '\0', 20) ;
		switch (segment_override)
		{
			case ES: sprintf(segment, "es") ; break ;
			case CS: sprintf(segment, "cs") ; break ;
			case SS: sprintf(segment, "ss") ; break ;
			case DS: sprintf(segment, "ds") ; break ;
		}
		printf("%s %s", segment, s ) ; 
		segment_override = -1 ;
		rm_segment_override = -1 ; 
	}
	else
	{
		printf("%s", s) ; 
	}
}

void disasm(unsigned char *buffer, long num)
{
	int j = 0; 
	int z = 0 ;
	while (j < num)
	{
		unsigned int addr = j ;
		if (segment_override == -1)
			printf("%08X  ", addr) ;
		z = buffer[j] ;
		switch (buffer[j])
		{
			case 0x00: parse("add %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x01: parse("add %s\n", rm16_r16,buffer, &j) ; break ;
			case 0x02: parse("add %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x03: parse("add %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x04: parse("add al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x05: parse("add ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x06: parse_noop("push es\n", buffer, &j); break ; 
			case 0x07: parse_noop("pop es\n", buffer, &j); break ;
			case 0x08: parse("or %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x09: parse("or %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x0A: parse("or %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x0B: parse("or %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x0C: parse("or al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x0D: parse("or ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x0E: parse_noop("push cs\n", buffer, &j) ; break ;
			case 0x10: parse("adc %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x11: parse("adc %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x12: parse("adc %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x13: parse("adc %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x14: parse("adc al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x15: parse("adc ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x16: parse_noop("push ss\n", buffer, &j) ; break ;
			case 0x17: parse_noop("pop ss\n", buffer, &j) ; break ;
			case 0x18: parse("sbb %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x19: parse("sbb %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x1A: parse("sbb %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x1B: parse("sbb %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x1C: parse("sbb al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x1D: parse("sbb ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x1E: parse_noop("push ds\n", buffer, &j) ; break ;
			case 0x1F: parse_noop("pop ds\n", buffer, &j) ; break ;
			case 0x20: parse("and %s\n", rm8_r8, buffer, &j) ; break ;
			case 0x21: parse("and %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x22: parse("and %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x23: parse("and %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x24: parse("and al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x25: parse("and ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x26: 
			{
				segment_override = ES ; 
				rm_segment_override = ES;
			}
			break ;
			case 0x27: parse_noop("daa\n", buffer, &j) ; break ;
			case 0x28: parse("sub %s\n", rm8_r8, buffer, &j) ; break ;
			case 0x29: parse("sub %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x2A: parse("sub %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x2B: parse("sub %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x2C: parse("sub al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x2D: parse("sub ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x2E: 
			{
				segment_override = CS ; 
				rm_segment_override = CS ;
			} break ;
			case 0x2F: parse_noop("das\n", buffer, &j) ; break ;
			case 0x30: parse("xor %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x31: parse("xor %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x32: parse("xor %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x33: parse("xor %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x34: parse("xor al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x35: parse("xor ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x36: 
			{
				segment_override = SS ; 
				rm_segment_override = SS ; 
			} break ;
			case 0x37: parse_noop("aaa\n", buffer, &j) ; break ;
			case 0x38: parse("cmp %s\n", rm8_r8, buffer,&j) ; break ;
			case 0x39: parse("cmp %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x3A: parse("cmp %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x3B: parse("cmp %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x3C: parse("cmp al,%s\n", imm8, buffer, &j) ; break ; 
			case 0x3D: parse("cmp ax,%s\n", imm16, buffer, &j) ; break ;
			case 0x3E: 
			{
				segment_override = DS ; 
				rm_segment_override = DS ; 
			}
			break ;
			case 0x3F: parse_noop("ass\n", buffer, &j) ; break ;
			case 0x40: parse_noop("inc ax\n", buffer, &j) ; break ;
			case 0x41: parse_noop("inc cx\n", buffer, &j) ; break ;
			case 0x42: parse_noop("inc dx\n", buffer, &j) ; break ;
			case 0x43: parse_noop("inc bx\n", buffer, &j) ; break ;
			case 0x44: parse_noop("inc sp\n", buffer, &j) ; break ;
			case 0x45: parse_noop("inc bp\n", buffer, &j) ; break ;
			case 0x46: parse_noop("inc si\n", buffer, &j) ; break ;
			case 0x47: parse_noop("inc di\n", buffer, &j) ; break ;
			case 0x48: parse_noop("dec ax\n", buffer, &j) ; break ;
			case 0x49: parse_noop("dec cx\n", buffer, &j) ; break ;
			case 0x4A: parse_noop("dec dx\n", buffer, &j) ; break ;
			case 0x4B: parse_noop("dec bx\n", buffer, &j) ; break ;
			case 0x4C: parse_noop("dec sp\n", buffer, &j) ; break ;
			case 0x4D: parse_noop("dec bp\n", buffer, &j) ; break ;
			case 0x4E: parse_noop("dec si\n", buffer, &j) ; break ;
			case 0x4F: parse_noop("dec di\n", buffer, &j) ; break ;
			case 0x50: parse_noop("push ax\n", buffer, &j) ; break ;
			case 0x51: parse_noop("push cx\n", buffer, &j) ; break ;
			case 0x52: parse_noop("push dx\n", buffer, &j) ; break ;
			case 0x53: parse_noop("push bx\n", buffer, &j) ; break ;
			case 0x54: parse_noop("push sp\n", buffer, &j) ; break ;
			case 0x55: parse_noop("push bp\n", buffer, &j) ; break ;
			case 0x56: parse_noop("push si\n", buffer, &j) ; break ;
			case 0x57: parse_noop("push di\n", buffer, &j) ; break ;
			case 0x58: parse_noop("pop ax\n", buffer, &j) ; break ;
			case 0x59: parse_noop("pop cx\n", buffer, &j) ; break ;
			case 0x5A: parse_noop("pop dx\n", buffer, &j) ; break ;
			case 0x5B: parse_noop("pop bx\n", buffer, &j) ; break ;
			case 0x5C: parse_noop("pop sp\n", buffer, &j) ; break ;
			case 0x5D: parse_noop("pop bp\n", buffer, &j) ; break ;
			case 0x5E: parse_noop("pop si\n", buffer, &j) ; break ;
			case 0x5F: parse_noop("pop di\n", buffer, &j) ; break ;
			case 0x70: parse("jo %s\n", rel8, buffer, &j) ; break ;
			case 0x71: parse("jno %s\n", rel8, buffer, &j) ; break ;
			case 0x72: parse("jc %s\n", rel8, buffer, &j) ; break ;
			case 0x73: parse("jnc %s\n", rel8, buffer, &j) ; break ;
			case 0x74: parse("jz %s\n", rel8, buffer, &j) ; break ;
			case 0x75: parse("jnz %s\n", rel8, buffer, &j) ; break ;
			case 0x76: parse("jna %s\n", rel8, buffer, &j) ; break ;
			case 0x77: parse("ja %s\n", rel8, buffer, &j) ; break ;
			case 0x78: parse("js %s\n", rel8, buffer, &j) ; break ;
			case 0x79: parse("jns %s\n", rel8, buffer, &j) ; break ;
			case 0x7A: parse("jpe %s\n", rel8, buffer, &j) ; break ;
			case 0x7B: parse("jpo %s\n", rel8, buffer, &j) ; break ;
			case 0x7C: parse("jl %s\n", rel8, buffer, &j) ; break ;
			case 0x7D: parse("jnl %s\n", rel8, buffer, &j) ; break ;
			case 0x7E: parse("jng %s\n", rel8, buffer, &j) ; break ;
			case 0x7F: parse("jg %s\n", rel8, buffer, &j) ; break ;
			case 0x80:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ; 
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("add %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x01: parse("or %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x02: parse("adc %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x03: parse("sbb %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x04: parse("and %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x05: parse("sub %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x06: parse("xor %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x07: parse("cmp %s\n", rm8_imm8, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0x81:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ; 
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("add %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x01: parse("or %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x02: parse("adc %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x03: parse("sbb %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x04: parse("and %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x05: parse("sub %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x06: parse("xor %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x07: parse("cmp %s\n", rm16_imm16, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0x83:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("add %s\n", rm16_imm8, buffer, &j) ; break ;
					case 0x02: parse("adc %s\n", rm16_imm8, buffer, &j) ; break ;
					case 0x03: parse("sbb %s\n", rm16_imm8, buffer, &j) ; break ;
					case 0x05: parse("sub %s\n", rm16_imm8, buffer, &j) ; break ;
					case 0x07: parse("cmp %s\n", rm16_imm8, buffer, &j) ; break ;
					default: t = 1; break ;
				} 
				if (t) goto print_symbol ;
			} break ;
			case 0x84: parse("test %s\n", rm8_r8, buffer, &j) ; break ;
			case 0x85: parse("test %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x86: parse("xchg %s\n", rm8_r8, buffer, &j) ; break ;
			case 0x87: parse("xchg %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x88: parse("mov %s\n", rm8_r8, buffer, &j) ; break ;
			case 0x89: parse("mov %s\n", rm16_r16, buffer, &j) ; break ;
			case 0x8A: parse("mov %s\n", r8_rm8, buffer, &j) ; break ;
			case 0x8B: parse("mov %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x8C: 
			{ 
				parse("mov %s\n", rm16_sreg, buffer, &j) ; break ;
			}
			case 0x8D: parse("lea %s\n", r16_rm16, buffer, &j) ; break ;
			case 0x8E: 
			{
					parse("mov %s\n", sreg_rm16, buffer, &j) ; break ;
			}
			case 0x8F:
			{
			  	unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ; 
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("pop word %s\n", m16, buffer, &j) ; break ;
					default: t = 1; break ;
				}	
				if (t) goto print_symbol ;
			} break ;
			case 0x90: parse_noop("xchg ax,ax\n", buffer, &j) ; break ;
			case 0x91: parse_noop("xchg cx,ax\n", buffer, &j) ; break ;
			case 0x92: parse_noop("xchg dx,ax\n", buffer, &j) ; break ;
			case 0x93: parse_noop("xchg bx,ax\n", buffer, &j) ; break ;
			case 0x94: parse_noop("xchg sp,ax\n", buffer, &j) ; break ;
			case 0x95: parse_noop("xchg bp,ax\n", buffer, &j) ; break ;
			case 0x96: parse_noop("xchg si,ax\n", buffer, &j) ; break ;
			case 0x97: parse_noop("xchg di,ax\n", buffer, &j) ; break ;
			case 0x98: parse_noop("cbw\n", buffer, &j) ; break ;
			case 0x99: parse_noop("cwd\n", buffer, &j) ; break ;
			case 0x9A: parse("call %s\n", call_inter, buffer, &j) ; break ; 
			case 0x9B: parse_noop("wait\n", buffer, &j) ; break ;
			case 0x9C: parse_noop("pushf\n", buffer, &j) ; break ;
			case 0x9D: parse_noop("popf\n", buffer, &j) ; break ;
			case 0x9E: parse_noop("sahf\n", buffer, &j) ; break ;
			case 0x9F: parse_noop("lahf\n", buffer, &j) ; break ;
			case 0xA0: parse("mov al,%s\n", moffs16, buffer, &j) ; break ;
			case 0xA1: parse("mov ax,%s\n", moffs16, buffer, &j) ; break ;
			case 0xA2: parse("mov %s,al\n", moffs16, buffer, &j) ; break ;
			case 0xA3: parse("mov %s,ax\n", moffs16, buffer, &j) ; break ;
			case 0xA4: parse_noop("movsb\n", buffer, &j) ; break ;
			case 0xA5: parse_noop("movsw\n", buffer, &j) ; break ;
			case 0xA6: parse_noop("cmpsb\n", buffer, &j) ; break ;
			case 0xA7: parse_noop("cmpsw\n", buffer, &j) ; break ;
			case 0xA8: parse("test al, %s\n", imm8, buffer, &j) ; break ;
			case 0xA9: parse("test ax, %s\n", imm16, buffer, &j) ; break ;
			case 0xAA: parse_noop("stosb\n", buffer, &j) ; break ;
			case 0xAB: parse_noop("stosw\n", buffer, &j) ; break ;
			case 0xAC: parse_noop("lodsb\n", buffer, &j) ; break ;
			case 0xAD: parse_noop("lodsw\n", buffer, &j) ; break ;
			case 0xAE: parse_noop("scasb\n", buffer, &j) ; break ;
			case 0xAF: parse_noop("scasw\n", buffer, &j) ; break ;
			case 0xB0: parse("mov al,%s\n",imm8, buffer, &j); break;
			case 0xB1: parse("mov cl,%s\n",imm8, buffer, &j); break;
			case 0xB2: parse("mov dl,%s\n",imm8, buffer, &j); break;
			case 0xB3: parse("mov bl,%s\n",imm8, buffer, &j); break;
			case 0xB4: parse("mov ah,%s\n",imm8, buffer, &j); break;
			case 0xB5: parse("mov ch,%s\n",imm8, buffer, &j); break;
			case 0xB6: parse("mov dh,%s\n",imm8, buffer, &j); break;
			case 0xB7: parse("mov bh,%s\n",imm8, buffer, &j); break;
			case 0xB8: parse("mov ax,%s\n",imm16, buffer, &j); break;
			case 0xB9: parse("mov cx,%s\n",imm16, buffer, &j); break;
			case 0xBA: parse("mov dx,%s\n",imm16, buffer, &j); break;
			case 0xBB: parse("mov bx,%s\n",imm16, buffer, &j); break;
			case 0xBC: parse("mov sp,%s\n",imm16, buffer, &j); break;
			case 0xBD: parse("mov bp,%s\n",imm16, buffer, &j); break;
			case 0xBE: parse("mov si,%s\n",imm16, buffer, &j); break;
			case 0xBF: parse("mov di,%s\n",imm16, buffer, &j); break;
			case 0xC2: parse("ret %s\n", imm16, buffer, &j) ; break ; 
			case 0xC3: parse_noop("ret\n", buffer, &j) ; break ;
			case 0xC4: parse("les %s\n", r16_rm16, buffer, &j) ; break ;
			case 0xC5: parse("lds %s\n", r16_rm16, buffer, &j) ; break ;
			case 0xC6: parse("mov %s\n", rm16_imm8, buffer, &j) ; break ;
			case 0xC7: parse("mov %s\n", rm16_imm16, buffer, &j) ; break ;
			case 0xCA: parse("retf %s\n", imm16, buffer, &j) ; break ;
			case 0xCB: parse_noop("retf\n", buffer, &j) ; break ;
			case 0xCC: parse_noop("int3\n", buffer, &j) ; break ;
			case 0xCD: parse("int %s\n", imm8, buffer, &j) ; break ;
			case 0xCE: parse_noop("into\n", buffer, &j) ; break ;
			case 0xCF: parse_noop("iret\n", buffer, &j) ; break ;
			case 0xD0:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("rol %s,1\n", rm8, buffer, &j) ; break ;
					case 0x01: parse("ror %s,1\n", rm8, buffer, &j) ; break ;
					case 0x02: parse("rcl %s,1\n", rm8, buffer, &j) ; break ;
					case 0x03: parse("rcr %s,1\n", rm8, buffer, &j) ; break ;
					case 0x04: parse("shl %s,1\n", rm8, buffer, &j) ; break ;
					case 0x05: parse("shr %s,1\n", rm8, buffer, &j) ; break ;
					case 0x07: parse("sar %s,1\n", rm8, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD1:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;	
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("rol %s,1\n", rm16, buffer, &j) ; break ;
					case 0x01: parse("ror %s,1\n", rm16, buffer, &j) ; break ;
					case 0x02: parse("rcl %s,1\n", rm16, buffer, &j) ; break ;
					case 0x03: parse("rcr %s,1\n", rm16, buffer, &j) ; break ;
					case 0x04: parse("shl %s,1\n", rm16, buffer, &j) ; break ;
					case 0x05: parse("shr %s,1\n", rm16, buffer, &j) ; break ;
					case 0x07: parse("sar %s,1\n", rm16, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD2:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;	
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("rol %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x01: parse("ror %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x02: parse("rcl %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x03: parse("rcr %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x04: parse("shl %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x05: parse("shr %s,cl\n", rm8, buffer, &j) ; break ;
					case 0x07: parse("sar %s,cl\n", rm8, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD3:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;	
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("rol %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x01: parse("ror %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x02: parse("rcl %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x03: parse("rcr %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x04: parse("shl %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x05: parse("shr %s,cl\n", rm16, buffer, &j) ; break ;
					case 0x07: parse("sar %s,cl\n", rm16, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xD4: parse_noop("aam\n", buffer, &j) ; break ;
			case 0xD5: parse_noop("aad\n", buffer, &j) ; break ;
			case 0xD7: parse_noop("xlatb\n", buffer, &j) ; break ;
			/*D8-DF => ESC0-7*/
			case 0xE0: parse("loopne %s\n", rel8, buffer, &j) ; break ;
			case 0xE1: parse("loope %s\n", rel8, buffer, &j) ; break ;
			case 0xE2: parse("loop %s\n", rel8, buffer, &j) ; break ;
			case 0xE3: parse("jcxz %s\n", rel8, buffer, &j) ; break ;
			case 0xE4: parse("in al,%s\n", imm8, buffer, &j) ; break ;
			case 0xE5: parse("in ax,%s\n", imm8, buffer, &j) ; break ;
			case 0xE6: parse("out %s,al\n", imm8, buffer, &j) ; break ;
			case 0xE7: parse("out %s,ax\n", imm8, buffer, &j) ; break ;
			case 0xE8: parse("call %s\n", rel16, buffer, &j) ; break ;
			case 0xE9: parse("jmp %s\n", rel16, buffer, &j) ; break ;
			case 0xEA: parse("jmp %s\n", call_inter, buffer, &j) ; break ;
			case 0xEB: parse("jmp short %s\n", rel8, buffer, &j) ; break ;
			case 0xEC: parse_noop("in al,dx\n", buffer, &j) ; break ;
			case 0xED: parse_noop("in ax,dx\n", buffer, &j) ; break ;
			case 0xEE: parse_noop("out dx,al\n", buffer, &j) ; break ;
			case 0xEF: parse_noop("out dx,ax\n", buffer, &j) ; break ;
			case 0xF0: printf("lock ") ; break ;
			case 0xF2: printf("repne ") ; break ;
			case 0xF3: printf("rep ") ; break ;
			case 0xF4: parse_noop("hlt\n", buffer, &j) ; break ;
			case 0xF5: parse_noop("cmc\n", buffer, &j) ; break ;
			case 0xF6:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;	
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("test %s\n", rm8_imm8, buffer, &j) ; break ;
					case 0x02: parse("not %s\n", rm8, buffer, &j) ; break ;
					case 0x03: parse("neg %s\n", rm8, buffer, &j) ; break ;
					case 0x04: parse("mul %s\n", rm8, buffer, &j) ; break ;
					case 0x05: parse("imul %s\n", rm8, buffer, &j) ; break ;
					case 0x06: parse("div %s\n", rm8, buffer, &j) ; break ;
					case 0x07: parse("idiv %s\n", rm8, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xF7:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				unsigned char t = 0 ;
				j-- ;	
				switch (opcode)
				{
					case 0x00: parse("test %s\n", rm16_imm16, buffer, &j) ; break ;
					case 0x02: parse("not %s\n", rm16, buffer, &j) ; break ;
					case 0x03: parse("neg %s\n", rm16, buffer, &j) ; break ;
					case 0x04: parse("mul %s\n", rm16, buffer, &j) ; break ;
					case 0x05: parse("imul %s\n", rm16, buffer, &j) ; break ;
					case 0x06: parse("div %s\n", rm16, buffer, &j) ; break ;
					case 0x07: parse("idiv %s\n", rm16, buffer, &j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xF8: parse_noop("clc\n", buffer, &j) ; break ;
			case 0xF9: parse_noop("stc\n", buffer, &j) ; break ;
			case 0xFA: parse_noop("cli\n", buffer, &j) ; break ;
			case 0xFB: parse_noop("sti\n", buffer, &j) ; break ;
			case 0xFC: parse_noop("cld\n", buffer, &j) ; break ;
			case 0xFD: parse_noop("std\n", buffer, &j) ; break ;
			case 0xFE:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;
				unsigned char t = 0 ;	
				switch (opcode)
				{
					case 0x00: parse("inc %s\n", rm8, buffer,&j) ; break ;
					case 0x01: parse("dec %s\n", rm8, buffer,&j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			case 0xFF:
			{
				unsigned char opcode = ((buffer[++j] & 0x38) >> 3 ); 
				j-- ;	
				unsigned char t = 0 ;
				switch (opcode)
				{
					case 0x00: parse("inc %s\n", rm16, buffer,&j) ; break ;
					case 0x01: parse("dec %s\n", rm16, buffer,&j) ; break ;
					case 0x02: parse("call near %s\n", rm16, buffer,&j) ; break ;
					case 0x03: parse("call far %s\n", rm16, buffer,&j) ; break ;
					case 0x04: parse("jmp near %s\n", rm16, buffer,&j) ; break ;
					case 0x05: parse("jmp far %s\n", rm16, buffer,&j) ; break ;
					case 0x06: parse("push %s\n", rm16, buffer,&j) ; break ;
					default: t = 1; break ;
				}
				if (t) goto print_symbol ;
			} break ;
			print_symbol:
			default: 
			{
				char tmp_buffer[20] ; 
				memset(tmp_buffer, '\0', 20) ;
				sprintf(tmp_buffer, "db 0x%X\n", buffer[j]) ;
				parse_noop(tmp_buffer, buffer, &j) ;
				break ;
			}
		}
		j++ ;
	}
}

char str[255] ; 

char *moffs16(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	char segment[10] ;
	memset(segment, '\0', 10) ;
	if (segment_override >= 0)
	{
		switch (segment_override)
		{
			case ES: sprintf(segment, "es:") ; break ;
			case CS: sprintf(segment, "cs:") ; break ;
			case SS: sprintf(segment, "ss:") ; break ;
			case DS: sprintf(segment, "ds:") ; break ;
		}
		segment_override = -1 ;
	}
	if (get_bytes(2, *j))
	{
		*err = 1 ; 
		return str ; 
	}
	(*j)++ ; 
	bytes++ ;
	unsigned char low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ; 
	unsigned char high = buffer[*j] ; 
	unsigned short imm16 = ((high << 8) + low) ;
	sprintf(str, "[%s0x%x]", segment, imm16) ;
	return str ;
}

char *rm8(char *buffer, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	char *s =  rm(buffer, j, 8, &error)  ;
	if (error)
	{
		*err = 1 ;
		return str ;
	}
	sprintf(str, "%s", s) ; 
	return str ;
}

char *rm16(char *buffer, int *j, int *err)
{
	int error = 0; 
	memset(str, '\0', 255) ;
	char *s =  rm(buffer, j, 16, &error)  ;
	if (error)
	{
		*err = error ;
		return str; 
	}
	sprintf(str, "%s", s) ; 
	return str ;
}

char *call_inter(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(4, *j))
	{
		*err = 1 ; 
		return str ;
	}
	(*j)++ ;
	bytes++ ; 
	unsigned char offset_low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ;
	unsigned char offset_high = buffer[*j] ; 
	(*j)++ ;
	bytes++ ;
	unsigned char seg_low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ;
	unsigned char seg_high = buffer[*j] ; 
	unsigned short offset = ((offset_high << 8) + offset_low) ; 
	unsigned short seg = ((seg_high << 8) + seg_low) ;
	sprintf(str,"0x%x:0x%x", seg, offset) ;
	return str ;	
}
char *m16(char *buffer, int *j, int *err)
{
	int error = 0; 
	memset(str, '\0', 255) ;
	char *s =  rm(buffer, j, 16, &error)  ;
	if (error)
	{
		*err = 1 ;
		return str ;
	}
	sprintf(str,"%s", s) ;
	return str ;
}
char *sreg_rm16(char *buffer, int *j, int *error) 
{
	int err = 0 ;
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j))
	{
		*error = 1 ; 
		return str ;
	}
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3) ;
	(*j)-- ;
	char *s =  rm(buffer, j, 16, &err)  ;
	if (err)
	{
		*error = 1 ;
		return str ;
	}
	if (reg < 4)
	{
		char *sreg = segreg[reg] ;
		sprintf(str,"%s,%s", sreg, s) ;
		*error = 0 ;
	} else *error = 1 ; 
	return str ;
}

char *rm16_sreg(char *buffer, int *j, int *error)
{
	int err = 0; 
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j))
	{
		*error = 1 ; 
		return str ;
	}
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3) ;
	(*j)-- ; 
	char *s =  rm(buffer, j, 16, &err)  ;
	if (err)
	{
		*error = 1 ;
		return str ;
	}
	if (reg < 4)
	{
		char *sreg = segreg[reg] ;
		sprintf(str,"%s,%s", s, sreg) ;
		*error = 0 ;
	} else *error = 1 ; 
	return str ;
}
char *rm16_imm8(char *buffer, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	char *s = rm(buffer, j, 16, &error) ;
	if (error)
	{
		*err = 1 ; 
		return str ;
	}
	(*j)++ ;
	if (get_bytes(1, *j))
	{
		(*j)--;
		*err = 1 ; 
		return str ;
	}
	(*j)--;
	(*j)++; 
	bytes++ ;
	signed char imm8 = buffer[*j] ; 
	char sign = '+' ;
	if (imm8 < 0) 
	{
		sign = '-' ;
		imm8 = -imm8 ;
	}
	sprintf(str, "%s,byte %c0x%x", s, sign, imm8) ;
	return str ;
}
char *rm16_imm16(char *buffer, int *j, int *err)
{
	int error = 0; 
	memset(str, '\0', 255) ;
	char *s = rm(buffer, j, 16, &error) ;
	if (error)
	{
		*err = 1 ; 
		return str ;
	}
	(*j)++ ;
	if (get_bytes(2, *j))
	{
		(*j)-- ;
		*err = 1 ;
		return str ;
	}
	(*j)-- ;
	(*j)++;
	bytes++ ;
	unsigned char low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ;
	unsigned char high = buffer[*j] ; 
	unsigned short imm16 = ((high << 8) + low) ; 
	sprintf(str, "%s,0x%x", s, imm16) ;
	return str ;
}
char *rm8_imm8(char *buffer, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ;
	char *s = rm(buffer, j, 8, &error) ;
	if (error)
	{
		*err = 1 ; 
		return str ;
	}
	(*j)++ ;
	if (get_bytes(1, *j))
	{
		(*j)-- ;
		*err = 1 ; 
		return str ;
	} 
	(*j)-- ; 
	(*j)++;
	bytes++ ;
	unsigned char imm8 = buffer[*j] ; 
	sprintf(str, "%s,0x%x", s, imm8) ;
	return str ;  
}

char *rel16(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(2, *j))
	{
		*err = 1 ;
		return str ;
	}
	(*j)++ ;
	bytes++ ;
	unsigned char rel_low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ; 
	unsigned char rel_high = buffer[*j] ; 
	signed short rel = ((rel_high << 8) + rel_low) ;
	unsigned short result = *j + rel + 1 ;
	sprintf(str, "0x%x", result) ; 
	return str ;
}


char *rel8(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j))
	{
		*err = 1 ;
		return str ;
	}
	(*j)++ ;
	bytes++ ;
	signed char rel = buffer[*j] ; 
	unsigned short result = *j + rel + 1 ;
	sprintf(str, "0x%x", result) ; 
	return str ;
}

char *imm8(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(1, *j))
	{
		*err = 1 ;
		return str ;
	}
	(*j)++ ;
	bytes++ ;
	unsigned char imm8 = buffer[*j] ; 
	sprintf(str, "0x%x", imm8) ; 
	return str ; 
}

char *imm16(char *buffer, int *j, int *err)
{
	memset(str, '\0', 255) ;
	if (get_bytes(2, *j))
	{
		*err = 1 ;
		return str ;
	}
	(*j)++ ; 
	bytes++ ;
	unsigned char low = buffer[*j] ; 
	(*j)++ ;
	bytes++ ;
	unsigned char high = buffer[*j] ; 
	unsigned short imm16 = ((high << 8) + low) ;
	sprintf(str, "0x%x", imm16); 
	return str ;
}

char *r16_rm16(char *buffer, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3 ); 
	(*j)-- ; 
	char *s = rm(buffer, j, 16, &error) ;
	if (error)
	{
		*err = 1 ; 
		return str ;
	}
	char *reg16 = regs16[reg] ; 
	sprintf(str, "%s,%s", reg16, s) ;
	return str ;
}

char *rm8_r8(char *buffer, int *j, int *err)
{
	int error = 0; 
	memset(str, '\0', 255) ;
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3 ); 
	(*j)-- ; 
	char *s = rm(buffer, j, 8, &error) ;
	if (error)
	{
		*err = 1 ;
		return str ;
	}
	char *reg8 = regs8[reg] ; 
	sprintf(str, "%s,%s", s, reg8) ;
	return str ;
}

char *r8_rm8(char *buffer, int *j, int *err)
{
	int error = 0 ;
	memset(str, '\0', 255) ;
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3 ); 
	(*j)-- ; 
	char *s = rm(buffer, j, 8, &error) ;
	if (error)
	{
		*err = 1 ;
		return str ;
	}
	char *reg8 = regs8[reg] ;
	sprintf(str, "%s,%s", reg8, s) ; 
	return str ;
}

char *rm16_r16(char *buffer, int *j, int *err)
{
	int error = 0;
	memset(str, '\0', 255) ; 
	unsigned char reg = ((buffer[++(*j)] & 0x38) >> 3 ); 
	(*j)-- ;
	char *s = rm(buffer, j, 16, &error) ;
	if (error)
	{
		*err = 1 ;
		return str ;
	}
	char *reg16 = regs16[reg] ; 
	sprintf(str, "%s,%s", s, reg16) ;
	return str ;
}

char rm_str[255] ; 

char *rm(char *buffer, int *j, char type, int *error)
{
	memset(rm_str, '\0', 255) ; 
	bytes++ ;
	if (get_bytes(1, *j))
	{
		bytes = 1 ;
		*error = 1; 
		return rm_str ;  
	}
	unsigned char rm_byte = buffer[++(*j)] ; 
	unsigned char mod = (rm_byte >> 6) ; 
	unsigned char rm8 = (rm_byte & 7) ; 
	char disp_str[255] ; 
	memset(rm_str, '\0', 255) ; 
	memset(disp_str, '\0', 255) ;
	char segment[10] ;
	memset(segment, '\0', 10) ;
	if (segment_override >= 0)
	{
		switch (segment_override)
		{
			case ES: sprintf(segment, "es:") ; break ;
			case CS: sprintf(segment, "cs:") ; break ;
			case SS: sprintf(segment, "ss:") ; break ;
			case DS: sprintf(segment, "ds:") ; break ;
		}
	}
	switch (mod)
	{
		case 0x0:
		{
		if (rm8 == 0x06)
		{
			if (get_bytes(2, *j))
			{
				*error = 1 ;
				(*j)-- ; 
				return rm_str ;  
			}
			(*j)++ ; 
			bytes++ ;
			unsigned char low = buffer[*j] ;
			(*j)++ ; 
			bytes++ ;
			unsigned char high = buffer[*j] ; 
			unsigned short disp = ((high << 8) + low) ; 
			char sign = '+' ;
			sprintf(disp_str, "%c0x%x", sign, disp) ;
		}
		else sprintf(disp_str, "") ;
		} break ; 
		case 0x01:
		{
			if (get_bytes(1, *j))
			{
				*error = 1;
				(*j)-- ;
				return rm_str ;  
			}
			signed char disp_low = buffer[++(*j)] ; 
			signed short disp = disp_low ; 
			bytes++ ;
			char sign = '+' ; 
			if (disp < 0) 
			{
				sign = '-' ;
				disp = ~disp ;
				disp++ ;
			}
			sprintf(disp_str, "%c0x%x", sign, disp) ; 
		} break ;
		case 0x02:
		{
			if (get_bytes(2, *j))
			{
				*error = 1;
				(*j)-- ; 
				return rm_str ;  
			}
			(*j)++ ; 
			bytes++ ;
			unsigned char low = buffer[*j] ;
			(*j)++ ; 
			bytes++ ;
			unsigned char high = buffer[*j] ;
			unsigned short disp = ((high << 8) + low) ; 
			char sign = '+' ;
			sprintf(disp_str, "%c0x%x", sign, disp) ;
		} break ;
		case 0x03:
		{
			if (type == 8)
			{
 				return regs8[rm8] ;
			}
			if (type == 16)
			{
				return regs16[rm8]; 
			}
		} break ;
	}
	switch (rm8)
	{
		case 0x00: sprintf(rm_str, "[%sbx+si%s]", segment, disp_str) ; break ;
		case 0x01: sprintf(rm_str, "[%sbx+di%s]", segment, disp_str) ; break ;
		case 0x02: sprintf(rm_str, "[%sbp+si%s]", segment, disp_str) ; break ;
		case 0x03: sprintf(rm_str, "[%sbp+di%s]", segment, disp_str) ; break ;
		case 0x04: sprintf(rm_str, "[%ssi%s]", segment, disp_str) ; break ;
		case 0x05: sprintf(rm_str, "[%sdi%s]", segment, disp_str) ; break ;
		case 0x06: sprintf(rm_str, "[%sbp%s]", segment, disp_str) ; break ; 
		case 0x07: sprintf(rm_str, "[%sbx%s]", segment, disp_str) ; break ;
	}
	return rm_str ; 
}

char* read_file(char *name, long *num)
{
	FILE *fp ;
	char *buffer;
	fp = fopen(name, "rb") ;
	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END) ;
		*num = ftell(fp) ;
		rewind(fp) ;
		buffer = (char*) malloc(sizeof(char) * (*num)) ;
		if (buffer == NULL)
		{
			printf("Memory error!\n") ;
			exit(1) ;
		}
		long result = fread(buffer, 1, *num, fp) ;
		if (result != *num)
		{
			printf("File read error!\n") ;
			exit(1) ;
		}
		num_bytes  = *num ;
		return buffer; 
	}
	else
	{
		printf("Error opening file!\n") ;
		exit(1) ;
	}
}