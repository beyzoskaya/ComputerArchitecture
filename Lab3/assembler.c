#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *input,*output;

    input = fopen(argv[1],"r");
    output = fopen("out.hex","w");

    if( input == NULL || output == NULL ) {
        fprintf(stderr,"Couldn't access file\n");
        return -1;
    }

    char buffer[100],instruction[10];
    uint32_t opcode,rd,funct3,rs1,rs2,imm,funct7,shamt,hex;

    while(fgets(buffer,100,input) != NULL)
    {
        sscanf(buffer,"%s",instruction);

            if(!strcmp(instruction,"add")){
                sscanf(buffer,"%*s x%d, x%d,x%d",&rd, &rs1,&rs2);
                opcode = 51;
                funct3 = 0;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }
            if(!strcmp(instruction,"sub")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 0;
                funct7 = 32;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"and")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 7;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"or")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 6;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"xor")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 4;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"sll")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 1;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"srl")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 5;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }

            if(!strcmp(instruction,"slt")){
                sscanf(buffer,"%*s x%d, x%d, x%d",&rd, &rs1, &rs2);
                opcode = 51;
                funct3 = 2;
                funct7 = 0;

                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (funct7 << 25);
            }


            if(!strcmp(instruction,"addi")){
                sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&imm);
                opcode = 19;
                funct3 = 0;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }

            if(!strcmp(instruction,"lui")){
                sscanf(buffer,"%*s x%d, %d",&rd,&imm);
                opcode = 55;
            
                hex = opcode | (rd << 7) | (imm << 12);
            }
            if(!strcmp(instruction,"andi")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&imm);
                opcode = 19;
                funct3 = 7;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }
            if(!strcmp(instruction,"ori")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&imm);
                opcode = 19;
                funct3 = 6;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }
            if(!strcmp(instruction,"xori")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&imm);
                opcode = 19;
                funct3 = 4;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }


            if(!strcmp(instruction,"slli")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&shamt);
                opcode = 19;
                funct3 = 1;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (shamt << 20);
            }

            if(!strcmp(instruction,"srli")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&shamt);
                opcode = 19;
                funct3 = 5;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (shamt << 20);
            }

            if(!strcmp(instruction,"slti")){
               sscanf(buffer,"%*s x%d,x%d",&rd, &rs1,&imm);
                opcode = 19;
                funct3 = 2;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }

                
            if(!strcmp(instruction,"lw")){
                sscanf(buffer,"%*s x%d, %d(x%d)",&rd,&imm,&rs1);
                opcode = 3;
                funct3 = 0;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }

            if(!strcmp(instruction,"lb")){
                sscanf(buffer,"%*s x%d, %d(x%d)",&rd,&imm,&rs1);
                opcode = 3;
                funct3 = 0;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }

            if(!strcmp(instruction,"lh")){
                sscanf(buffer,"%*s x%d, %d(x%d)",&rd,&imm,&rs1);
                opcode = 3;
                funct3 = 1;
            
                hex = opcode | (rd << 7) | (funct3 << 12) | (rs1 << 15) | (imm << 20);
            }

            if(!strcmp(instruction,"sw")){
                sscanf(buffer,"%*s x%d, %d(x%d)", &rs1,&rs2, &imm);
                opcode = 35;
                funct3 = 2;

                uint32_t imm4_0, imm11_5;
                imm4_0 = imm & 31;
                imm11_5 = (imm >> 5) & 127;
                
                hex = opcode | (imm4_0 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (imm11_5 << 25);
            }
            if(!strcmp(instruction,"sb")){
                sscanf(buffer,"%*s x%d, %d(x%d)", &rs2, &imm, &rs1);
                opcode = 35;
                funct3 = 0;

                uint32_t imm4_0, imm11_5;
                imm4_0 = imm & 31;
                imm11_5 = (imm >> 5) & 127;
                
                hex = opcode | (imm4_0 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (imm11_5 << 25);
            }

            if(!strcmp(instruction,"sh")){
                sscanf(buffer,"%*s x%d, %d(x%d)", &rs2, &imm, &rs1);
                opcode = 35;
                funct3 = 1;

                uint32_t imm4_0, imm11_5;
                imm4_0 = imm & 31;
                imm11_5 = (imm >> 5) & 127;
                
                hex = opcode | (imm4_0 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | (imm11_5 << 25);
            }

            if(!strcmp(instruction,"beq")){
                sscanf(buffer,"%*s x%d, %d", &rs1, &rs2 ,&imm);
                opcode = 99;
                funct3 = 0;

                uint32_t imm4_1, imm10_5,imm12;
                imm4_1 = (imm >> 1) & 15;
                imm10_5 = ((imm >> 5) & 63) << 25;
                imm12 = (imm >> 12) << 31;
                
                hex = opcode | (imm4_1 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | imm10_5  | imm12 ;
            }

            if(!strcmp(instruction,"bne")){
                sscanf(buffer,"%*s x%d, %d", &rs1, &rs2 ,&imm);
                opcode = 99;
                funct3 = 1;

                uint32_t imm4_1, imm10_5,imm12;
                imm4_1 = (imm >> 1) & 15;
                imm10_5 = ((imm >> 5) & 63) << 25;
                imm12 = (imm >> 12) << 31;
                
                hex = opcode | (imm4_1 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | imm10_5  | imm12 ;
            }

            if(!strcmp(instruction,"blt")){
                sscanf(buffer,"%*s x%d, %d", &rs1, &rs2 ,&imm);
                opcode = 99;
                funct3 = 4;

                uint32_t imm4_1, imm10_5,imm12;
                imm4_1 = (imm >> 1) & 15;
                imm10_5 = ((imm >> 5) & 63) << 25;
                imm12 = (imm >> 12) << 31;
                
                hex = opcode | (imm4_1 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | imm10_5  | imm12 ;
            }

            if(!strcmp(instruction,"bge")){
                sscanf(buffer,"%*s x%d, %d", &rs1, &rs2 ,&imm);
                opcode = 99;
                funct3 = 5;

                uint32_t imm4_1, imm10_5,imm12;
                imm4_1 = (imm >> 1) & 15;
                imm10_5 = ((imm >> 5) & 63) << 25;
                imm12 = (imm >> 12) << 31;
                
                hex = opcode | (imm4_1 << 7) | (funct3 << 12) | (rs1 << 15) | (rs2 << 20) | imm10_5  | imm12 ;
            }

            if(!strcmp(instruction,"jal")){
            sscanf(buffer,"%*s x%d, %d", &rd, &imm);
            opcode = 111;

            uint32_t imm19_12, imm10_1, imm20, imm11;
            imm19_12 = (imm >> 12) & 255;
            imm10_1 = (imm >> 1) & 1023;
            imm20 = imm >> 20;
            imm11 = (imm >> 11) & 1;

            hex = opcode | (rd << 7) | (imm10_1 << 12) | (imm11 << 22) | (imm19_12 << 23) | (imm20 << 31); 
            }

            if(!strcmp(instruction,"ecall")){
                opcode = 115;
                hex = opcode;
            }
            

            fprintf(output,"%08x\n",hex);

    }
   

    fclose(input);
    fclose(output);

    return 0;

}