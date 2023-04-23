#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "ozu-riscv32.h"

/***************************************************************/
/* Print out a list of commands available                      */
/***************************************************************/
void help() {        
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********OZU-RV32 Disassembler and Simulator Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                              */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                               */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                           */
/***************************************************************/
void cycle() {                                                
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate RISC-V for n cycles                                */
/***************************************************************/
void run(int num_cycles) {                                      
	
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                      */
/***************************************************************/
void runAll() {                                                     
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/**************************************************************************************/ 
/* Dump region of memory to the terminal (make sure provided address is word aligned) */
/**************************************************************************************/
void mdump(uint32_t start, uint32_t stop) {          
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal             */   
/***************************************************************/
void rdump() {                               
	int i; 
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < RISCV_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	
}

/***************************************************************/
/* Read a command from standard input.                         */  
/***************************************************************/
void handle_command() {                         
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;

	printf("OZU-RISCV SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll(); 
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting OZU-RISCV! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'P':
		case 'p':
			print_program(); 
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                   */
/***************************************************************/
void reset() {   
	int i;
	/*reset registers*/
	for (i = 0; i < RISCV_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
	
	/*load program*/
	load_program();
	
	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                             */
/***************************************************************/
void init_memory() {                                           
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                   */
/**************************************************************/
void load_program() {                   
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */
	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}
 
struct Package
{	
	uint32_t ins;
	uint32_t rd;
	uint32_t rs1;
	uint32_t rs2;
	uint32_t imm12;

	uint32_t add;
	uint32_t addi;
	uint32_t sub;
	uint32_t lui;
	uint32_t auipc;
	uint32_t and;
	uint32_t andi;
	uint32_t or;
	uint32_t ori;
	uint32_t xor;
	uint32_t xori;
	uint32_t sll;
	uint32_t slli;
	uint32_t srl;
	uint32_t srli;
	uint32_t slt;
	uint32_t slti;
	uint32_t mul;
	uint32_t div;
	uint32_t divu;
	uint32_t lw;
	uint32_t lb;
	uint32_t lh;
	uint32_t lbu;
	uint32_t lhu;
	uint32_t sw;
	uint32_t sb;
	uint32_t sh;
	uint32_t beq;
	u_int32_t bne;
	uint32_t blt;
	uint32_t bge;
	uint32_t bltu;
	uint32_t bgeu;
	uint32_t jal;
	uint32_t ecall;
	
};

struct Package Decoder(uint32_t current_i){
	struct Package pkt;
	 pkt.ins = current_i;
	 pkt.rd = (current_i >> 7) & 31;
	 pkt.rs1 = (current_i >> 15) & 31;
	 pkt.rs2 = (current_i >> 20) & 31;
	 pkt.imm12 = current_i >> 20;


	pkt.add = (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 0 + 51);
	pkt.sub = (current_i & 4261441663) ==  (pow(2,30)* 1 + pow(2,12) * 0 + 51);
	pkt.and = (current_i & 4261441663) ==  (pow(2,30)* 1 + pow(2,12) * 7 + 51);
	pkt.or =  (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 6 + 51);
	pkt.xor = (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 4 + 51);
	pkt.sll = (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 1+ 51);
	pkt.srl = (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 5 + 51);
	pkt.slt = (current_i & 4261441663) ==  (pow(2,30)* 0 + pow(2,12) * 2 + 51);
	pkt.mul = (current_i & 4261441663) ==  (pow(2,25)* 1 + pow(2,12) * 0 + 51);
	pkt.div = (current_i & 4261441663) ==  (pow(2,25)* 1 + pow(2,12) * 4 + 51);
	pkt.divu = (current_i & 4261441663) ==  (pow(2,25)* 1 + pow(2,12) * 5 + 51);
	



	pkt.addi = (current_i & 28799) == (pow(2,12) * 0 + 19);
	pkt.xori = (current_i & 28799) == (pow(2,12) * 4 + 19);
	pkt.slli = (current_i & 28799) == (pow(2,12) * 1 + 19);
	pkt.srli = (current_i & 28799) == (pow(2,12) * 5 + 19);
	pkt.slti = (current_i & 28799) == (pow(2,12) * 2 + 19);
	pkt.lw = (current_i & 28799) == (pow(2,12) * 2 + 3);
	pkt.lb = (current_i & 28799) == (pow(2,12) * 0 + 3);
	pkt.lh = (current_i & 28799) == (pow(2,12) * 1 + 3);
	pkt.lbu = (current_i & 28799) == (pow(2,12) * 4 + 3);
	pkt.lhu = (current_i & 28799) == (pow(2,12) * 5 + 3);
	pkt.sw = (current_i & 28799) == (pow(2,12) * 2 + 35);
	pkt.sb = (current_i & 28799) == (pow(2,12) * 0 + 35);
	pkt.sh = (current_i & 28799) == (pow(2,12) * 1 + 35);
	pkt.beq = (current_i & 28799) == (pow(2,12) * 0 + 99);
	pkt.bne = (current_i & 28799) == (pow(2,12) * 1 + 99);
	pkt.blt = (current_i & 28799) == (pow(2,12) * 4 + 99);
	pkt.bge = (current_i & 28799) == (pow(2,12) * 5 + 99);
	pkt.bltu = (current_i & 28799) == (pow(2,12) * 6 + 99);
	pkt.bgeu = (current_i & 28799) == (pow(2,12) * 7 + 99);

	pkt.lui = (current_i & 127) == (pow(2,12) * 0 + 55);
	pkt.jal = (current_i & 127) == (pow(2,12) * 0 + 111);
	pkt.auipc = (current_i & 127) == (pow(2,12) * 0 + 23);
	pkt.ecall =(current_i & 115);
	return pkt;

}

int32_t Sext(int desired_length, int start_length, uint32_t imm){
	if((imm >> (start_length-1 ) & 1)){
		uint32_t new_length = desired_length - start_length;
		uint32_t temp = ((uint32_t)(pow(2, new_length+1))-1)<< start_length;
		return imm | temp;
		
	}else
	 	return imm;
}

void Execute(struct Package pkt){
	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
		uint32_t offset_branch = (((pkt.ins >> 7) & 1) << 11) | (((pkt.ins >> 8) & 31) << 1) | (((pkt.ins >> 25) & 63) << 5) | ((pkt.ins >> 31) << 12);
		uint32_t offset_j = ((pkt.ins & 0x7FE00000) >> 20) | ((pkt.ins & 0x00100000) >> 9) | (pkt.ins & 0x000FF000) | ((pkt.ins & 0x80000000) >> 11);
		uint32_t offset_s = (( pkt.ins & (127 << 25)) >> 20) + ((pkt.ins & (31 << 7)) >> 7);
		int32_t offset_u = (pkt.ins & (0xFFFFF << 12));
	if(pkt.add){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] + CURRENT_STATE.REGS[pkt.rs2];
	} else if(pkt.addi){
		printf("non-signed-extended=%d, sigextended=%d\n", pkt.imm12,Sext(32,12,pkt.imm12));
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] + Sext(32,12,pkt.imm12);
	} else if(pkt.and){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] + CURRENT_STATE.REGS[pkt.rs2];	
	}else if(pkt.andi){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] + Sext(32,12,pkt.imm12);
	}else if (pkt.auipc){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.PC + (Sext(32,12,pkt.imm12) <<12);
	}else if(pkt.beq){
		if(CURRENT_STATE.REGS[pkt.rs1 == pkt.rs2]){
			NEXT_STATE.PC = CURRENT_STATE.PC + Sext(32,12,pkt.imm12);
		}
	}else if(pkt.bge){
		if((Sext(32,32,CURRENT_STATE.REGS[pkt.rs1])) >= Sext(32,32,CURRENT_STATE.REGS[pkt.rs2])){
		
			NEXT_STATE.PC = CURRENT_STATE.PC + Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
		}
	}else if(pkt.bgeu){
		if(CURRENT_STATE.REGS[pkt.rs1] >= CURRENT_STATE.REGS[pkt.rs2]){
			NEXT_STATE.PC = CURRENT_STATE.PC + Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
		}
	}else if(pkt.blt){
		if(CURRENT_STATE.REGS[pkt.rs1] <  Sext(32,32,CURRENT_STATE.REGS[pkt.rs2])){
			NEXT_STATE.PC = CURRENT_STATE.PC + Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
		}
	}else if(pkt.ecall){
		RUN_FLAG = FALSE;
	}else if(pkt.bltu){
		if(CURRENT_STATE.REGS[pkt.rs1]< CURRENT_STATE.REGS[pkt.rs2]){
			NEXT_STATE.PC = Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
		}
	}else if(pkt.bne){
		if(CURRENT_STATE.REGS[pkt.rs1] != CURRENT_STATE.REGS[pkt.rs2]){
			NEXT_STATE.PC = Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
		}
	}else if(pkt.div){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] / Sext(32,12,CURRENT_STATE.REGS[pkt.rs2]);
	}else if(pkt.divu){
		NEXT_STATE.REGS[pkt.rd] = CURRENT_STATE.REGS[pkt.rs1] / CURRENT_STATE.REGS[pkt.rs2];
	}else if(pkt.sb){
		//mem_write_32((CURRENT_STATE.REGS[pkt.rs1]+Sext(32,12,CURRENT_STATE.REGS[pkt.rs2])),)
	}
}
 
/************************************************************/
/* decode and execute instruction                           */ 
/************************************************************/
void handle_instruction()
{
	/*YOU NEED TO IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/
	uint32_t current_ins = mem_read_32(CURRENT_STATE.PC); //read program counter's next instruction and hold it inside
	uint32_t opcode = current_ins & 127;
	
	uint32_t funct3 = (current_ins >> 12) & 7;
	uint32_t rs1 = (current_ins >> 15) & 31;
	uint32_t rs2 = (current_ins >> 20) & 31;
	uint32_t funct7 = (current_ins >> 25) & 7;
	uint32_t imm12 = (current_ins >> 20);
	int32_t imm12_signed = imm12;
	struct  Package pkt;
	pkt = Decoder(current_ins);
	printf("This func is add = %d, sub = %d\n",pkt.add,pkt.sub);
	Execute(pkt);

	
	
	

}


/************************************************************/
/* Initialize Memory                                        */ 
/************************************************************/
void initialize() { 
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/**********************************************************************/
/* Print the program loaded into memory (in RISC-V assembly format)   */ 
/**********************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}

/******************************************************************************/
/* Print the instruction at given memory address (in RISC-V assembly format)  */
/******************************************************************************/
void print_instruction(uint32_t addr){
	/*YOU NEED TO IMPLEMENT THIS FUNCTION*/
		/*YOU NEED TO IMPLEMENT THIS*/
	/* execute one instruction at a time. Use/update CURRENT_STATE and and NEXT_STATE, as necessary.*/
	uint32_t current_ins = mem_read_32(addr);//read program counter's next instruction and hold it inside
	uint32_t opcode = current_ins & 127; // 7 bit 
	uint32_t rd = (current_ins & (31 << 7)) >> 7; // 5 bit 
	uint32_t funct3 = (current_ins & (7 << 12)) >> 12; // 3 bit
	uint32_t rs1 = (current_ins & (31 << 15)) >> 15; // 5 bit 
	uint32_t rs2 = (current_ins & (31 << 20)) >> 20; // 5 bit 
	
    
	
	
	if(opcode == 19 || opcode == 3) {
		uint32_t immediate = (current_ins & (4095 << 20)) >> 20;

		if(opcode == 19) {
			if(funct3 == 0) {
			// addi
				int32_t imm = Sext(32,12,immediate);
				
				printf("addi   x%d,   x%d,   %d\n",rd,rs1,imm);
				
			} else if(funct3 == 1) {
			//slli
				
				printf("slli   x%d,   x%d,   %d\n",rd,rs1,immediate);
			} else if(funct3 == 2) {
			//slti
				
				printf("slti   x%d,   x%d,   %d\n",rd,rs1,Sext(32,12,immediate));
			} else if(funct3 == 4) {
			//xori
				
				printf("xori   x%d,   x%d,   %d\n",rd,rs1,Sext(32,12,immediate));
			} else if(funct3 == 5) {
			//srli
				
				printf("srli   x%d,   x%d,   %d\n",rd,rs1,immediate);
			} else if(funct3 == 6) {
			//ori
				
				printf("ori   x%d,   x%d,   %d\n",rd,rs1,Sext(32,12,immediate));
			} else if(funct3 == 7) {
			//andi
				
				printf("ori   x%d,   x%d,   %d\n",rd,rs1,Sext(32,12,immediate));
			} 
		}

		if(opcode == 3) {
			int32_t offset = Sext(32,12,immediate);
			int32_t addr = CURRENT_STATE.REGS[rs1] + offset;
			int32_t value = mem_read_32(addr);

			if(funct3 == 0) {
				// lb
				
				printf("lb   x%d,   %d,(x%d)\n",rd,offset,rs1);
			} else if(funct3 == 1) {
				//lh
				
				printf("lh   x%d,   %d,(x%d)\n",rd,offset,rs1);
			} else if(funct3 == 2) {
				// lw
				
				printf("lw   x%d,   %d,(x%d)\n",rd,offset,rs1);
			} else if(funct3 == 4) {
				//lbu
				
				printf("lbu   x%d,   %d,(x%d)\n",rd,offset,rs1);
			} else if(funct3 == 5) {
				//lhu
				
				printf("lhu   x%d,   %d,(x%d)\n",rd,offset,rs1);
			}
		}
	}

	
	
	if(opcode == 51) {
		uint32_t funct7 = (current_ins & (127 << 25)) >> 25;
		if(funct3 == 0) {
			if(funct7 == 32) {
				// sub function
				
				printf("sub   x%d,   x%d,   x%d\n",rd,rs1,rs2);
			}

			if(funct7 == 0) {
				// add function
				printf("add   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			}

			if(funct7 == 1) {
				//mul
				printf("mul   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			}
		} else if(funct3 == 1) {
			// sll function
			printf("sll   x%d,   x%d,   x%d\n",rd,rs1,rs2); 
			
		} else if(funct3 == 2) {
			// slt function
			printf("slt   x%d,   x%d,   x%d\n",rd,rs1,rs2);
			
		} else if(funct3 == 3) {
			// sltu function
			printf("sltu   x%d,   x%d,   x%d\n",rd,rs1,rs2);
			
		} else if(funct3 == 4) {
			if(funct7 == 1) {
				//div
				printf("div   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			}

			if(funct7 == 0) {
				// xor
				printf("xor   x%d,   x%d,   x%d\n",rd,rs1,rs2); 
				
			}
			
		} else if(funct3 == 5) {
			if(funct7 == 0) {
				//srl
				printf("srl   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			}
			if(funct7 == 32) {
				// sra
				printf("sra   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			}
			if(funct7 == 1) {
				//divu
				printf("divu   x%d,   x%d,   x%d\n",rd,rs1,rs2);
				
			} 

		} else if(funct3 == 6) {
				// or
			printf("or   x%d,   x%d,   x%d\n",rd,rs1,rs2);
			
		} else if(funct3 == 7) {
			 // and
			printf("and   x%d,  x %d,   x%d\n",rd,rs1,rs2); 
			
		}	
	}

	

	if(opcode == 35) {
		uint32_t immediate = ((current_ins & (127 << 25)) >> 20) + ((current_ins & (31 << 7)) >> 7);
		int32_t offset = Sext(32,12,immediate);
    	uint32_t addr = CURRENT_STATE.REGS[rs1] + offset;


		if(funct3 == 0) {
			// sb
			printf("sb   x%d,    %d(x%d)\n",rs2,offset,rs1);
			
		} else if(funct3 == 1) {
			// sh
			
			printf("sh   x%d,    %d(x%d)\n",rs2,offset,rs1);
		} else if(funct3 == 2) {
			// sw
			printf("sw   x%d,    %d(x%d)\n",rs2,offset,rs1);
			
		}
	}

	

	if(opcode == 23 || opcode == 55) {
		int32_t immediate = (current_ins & (0xFFFFF << 12)); 

		if(opcode == 23) {
			//auipc
			
			printf("auipc    x%d,    %d\n", rd, immediate);
		}

		if(opcode == 55) {
			//lui
			
			printf("lui    x%d,    %d\n", rd, immediate);
		}
	}

	

	if(opcode == 99) {
		
		int32_t immediate = (((opcode & 0x80000000) >> 31 << 12) |  ((opcode & (0x3f<<25)) >> 25 << 5)  |  (opcode & (0x0000000F << 7) >> 6)  |   ((opcode & (0x1 << 7)) >> 7 << 11))<<1;
		
        int32_t offset = immediate; 


		if(funct3 == 0) {
			//beq
			printf("beq    x%d,    x%d,    %d\n", rs1, rs2, offset);;
			
		} else if(funct3 == 1) {
			//bne
			printf("bne    x%d,    x%d,    %d\n", rs1, rs2, offset);;
		} else if(funct3 == 4) {
			//blt
			printf("blt    x%d,    x%d,    %d\n", rs1, rs2, offset);;
		} else if(funct3 == 5) {
			//bge
			printf("bge    x%d,    x%d,    %d\n", rs1, rs2, offset);;
		} else if(funct3 == 6) {
			//bltu
			printf("bltu    x%d,    x%d,    %d\n", rs1, rs2, offset);;
		} else if(funct3 == 7) {
			//bgeu
			printf("bgeu    x%d,    x%d,    %d\n", rs1, rs2, offset);;
		}
	}



	if(opcode == 111) {
		// jal
		int32_t immediate = Sext(32,12,((current_ins & 0x7FE00000) >> 20) | ((current_ins & 0x00100000) >> 9) | (current_ins & 0x000FF000) | ((current_ins & 0x80000000) >> 11));

		
		printf("jal    %d,    %d\n", rd, immediate);
	}

	if(opcode == 115) {
		// ecall
		printf("ecall \n");
	}
}



/***************************************************************/
/* main()                                                      */
/***************************************************************/
int main(int argc, char *argv[]) {                              
	printf("\n********************************\n");
	printf("Welcome to OZU-RISCV SIMULATOR...\n");
	printf("*********************************\n\n");
	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
