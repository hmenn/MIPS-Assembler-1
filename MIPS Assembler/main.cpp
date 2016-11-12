/* Name: Hasan
 * Surname: MEN
 * Student ID: 131044009
 * Implemented Missing Instructions:
 * Implemented Pseudo Instructions:
*/
//  MIPS Assembler
//  Created by Young Seok Kim
//  Copyright © 2015 TonyKim. All rights reserved.
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <bitset>
#include <vector>
#include <cstdint>
using namespace std;

// data structures
struct label {
    char name[100];
    int32_t location;
};

// Global variables
label label_list[100];
int labelindex = 0;

int32_t instructions[1000];
int instr_index = 0;

int32_t datasection[1000];
int datasectionindex = 0;

// functions
void scanLabels(char *filename);
int countDataSection(char *filename);

void assemble(char *instruction);

void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct);
void makeI_type(int op, int rs, int rt, int addr);
void makeJ_type(int op, int addr);
void showInstructions();

int regToInt(char *reg);
int labelToIntAddr(char *label);
int immToInt(char *immediate);
char *int2bin(int a, char *buffer, int buf_size);
char *intTobin(int input);
/************************ MAIN ************************/
int main() {
    char filename[20] ="a.asm";
    int dataSectionSize = countDataSection(filename);
    scanLabels(filename);

    string line;
    ifstream inputfile(filename);
    if (inputfile.is_open())
    {
        while (getline(inputfile, line) )
        {
            // go to .text section
            if(strstr(line.c_str(),".text") != NULL){
                break;
            }
        }

        while (getline(inputfile, line) )
        {
            // if the line contains : then it is label
            // so skip it
            if (strstr(line.c_str(), ":") != NULL) {
                continue;
            } else {
                char assemblyCodeline[100];
                strcpy(assemblyCodeline, line.c_str());
                // convert the instruction to binary
                assemble(assemblyCodeline);
            }
        }

        inputfile.close();
    }
    else {
    	cout << "Unable to open file\nexiting...\n";
    	return -1;
	}

    showInstructions();
    return 0;

    FILE *f = fopen("output.mcode", "w");
    fprintf(f, "%s\n", intTobin(instr_index*4));
	fprintf(f, "%s\n", intTobin(dataSectionSize*4));

    int i;
    for (i=0; i<instr_index; i++) {
        fprintf(f, "%s\n", intTobin(instructions[i]));
    }

    for (i=0; i<datasectionindex; i++) {
		fprintf(f, "%s\n", intTobin(datasection[i]));
    }

    fclose(f);

    return 0;
}

void scanLabels(char *filename) {
    int datasizecnt = 0;
    int instruction_count = 0;

    string line;
    ifstream inputfile(filename);
    if (inputfile.is_open())
    {
        // finds the .data section and count the number of .word before .text section appears
        while (getline(inputfile, line))
        {
            if (strstr(line.c_str(),".data") != NULL){
                break;
            }
        }
        while (getline(inputfile, line)) {
            // Labels inside .data section.
            if (strstr(line.c_str(),".word") != NULL){
                label newLabel;
                char temp[100];
                strcpy(temp, line.c_str());
                strcpy(newLabel.name , strtok(temp,":"));
                newLabel.location = 0x10000000+(datasizecnt)*4;
                datasizecnt++;
                label_list[labelindex++]=newLabel;
            }
            if (strstr(line.c_str(),".text") != NULL){
                break;
            }
        }
        // Labels inside .text section (function labels)
        while (getline(inputfile, line)) {
            if (strstr(line.c_str(), ":") != NULL) {
                label newLabel;
                char temp[100];
                strcpy(temp, line.c_str());
                strcpy(newLabel.name , strtok(temp,":"));
                newLabel.location = 0x400000 + instruction_count*4;
                label_list[labelindex++]=newLabel;
            } else {
                char temp[100];
                strcpy(temp, line.c_str());

                char * one;
                char * two;
                char * three = NULL;
                char key[] = " ,\t";
                one = strtok(temp, key);
                if(one != NULL){
                    two = strtok(NULL,key);
                }
                else{
                  printf("scanLabels::empty line\n");
                	return;
                }
                if(two != NULL)
                    three = strtok(NULL,key);

                if (strcmp(one, "la") == 0){
                    int location = labelToIntAddr(three);
                    if ((location & 65535) == 0) {          // if lower 16bit address is 0x0000
                        instruction_count++;                // lui instruction
                    } else {
                        instruction_count += 2;             // lui instruction + ori instruction
                    }
                } else {
                    instruction_count++;
                }
            }
        }


        inputfile.close();
    }
    else cout << "Unable to open file\n";
}

int countDataSection(char *filename) {
    int count = 0;

    string line;
    ifstream inputfile(filename);
    if (inputfile.is_open())
    {
        // finds the .data section and count the number of .word before .text section appears
        while (getline(inputfile, line))
        {
            if(strstr(line.c_str(),".data") != NULL){
                break;
            }
        }
        while (getline(inputfile, line)) {
            if(strstr(line.c_str(),".text") != NULL){
                break;
            }
            if(strstr(line.c_str(),".word") != NULL){
                count++;
                char * one;
                char * two;
                char * three;
                char temp[100];
                strcpy(temp, line.c_str());

                if (strstr(temp,":") != NULL) {
                    one = strtok(temp,":");
                    two = strtok(NULL, " \t");
                    three = strtok(NULL, " \t");
                    datasection[datasectionindex++] = immToInt(three);
                } else {
                    one = strtok(temp, " \t");
                    two = strtok(NULL, " \t");
                    datasection[datasectionindex++] = immToInt(two);
                }

            }
        }
        inputfile.close();
    }
    else cout << "Unable to open file\n";
    return count;
}

/* Assembling Function */
void assemble(char *instruction){
    char key[] = " ,\t\r\n";
    char * one = NULL; // opcode
    char * two = NULL; // rd or rt
    char * three = NULL; // rs or rt
    char * four = NULL; // rt or immediate

    one = strtok(instruction,key);

    if (one == NULL) {
      printf("assemble::empty line\n");
      return;
    } else if(one != NULL) {
        two = strtok(NULL,key);
    }

    if(two != NULL)
        three = strtok(NULL,key);
    if(three != NULL)
        four = strtok(NULL,key);

    //four[strlen(four)-1]='\0';

    // printf("one: %s, Size: %d\n", one,(int)strlen(one));
    // printf("two: %s, Size: %d\n", two,(int)strlen(two));
    // printf("three: %s, Size: %d\n", three,(int)strlen(three));
    // printf("four: %s, Size: %d\n", four,(int)strlen(four));
    // printf("continuing...\n");

    // R-Type : OP-RS-RT-RD-SHMT-FUNC
    // I-Type : OP-RS-RT-IMM
    // J-Type : OP-ADDRESS

    if (strcmp(one, "add") == 0) {
      // ADD rd, rs, rt [R-type]
      makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 32);
  	} else if (strcmp(one, "addi") == 0) {
      // ADDI rt, rs, immediate [I-type]
      makeI_type(8, regToInt(three), regToInt(two), immToInt(four));
  	} else if (strcmp(one, "lbu") == 0) {
      // LBU rs, offset(base) [I-type]
      // LBU	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(36, regToInt(rs), regToInt(two), immToInt(offset));
  	} else if (strcmp(one, "lhu") == 0) {
      // LHU rs, offset(base) [I-type]
      // LHU	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(37,regToInt(rs),regToInt(two),immToInt(offset));
  	} else if (strcmp(one, "ll") == 0) {
      // LL rs, offset(base) [I-type]
      // LL	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(48,regToInt(rs),regToInt(two),immToInt(offset));
  	} else if (strcmp(one, "slt") == 0) {
      // SLT rd, rs, rt [R-type]
      // OP	rs	rt	rd	0	SLT <- machine
      // rd = (rs<rt) ? 1 : 0
      makeR_type(0,regToInt(three),regToInt(four),regToInt(two),0,42);
  	} else if (strcmp(one, "slti") == 0) {
      // SLTI rt, rs, immediate [I-type]
      // SLTI	rs	rt	immediate <- machine
      // rs = (rt<imm) ? 1 : 0
      makeI_type(10,regToInt(three),regToInt(two),immToInt(four));
  	} else if (strcmp(one, "sb") == 0) {
      // Store Byte
      // SB rs, offset(base) [I-type]
      // SB	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(40,regToInt(rs),regToInt(two),immToInt(offset));
  	} else if (strcmp(one, "sc") == 0) {
      // Store Conditional Word
      // SC rs, offset(base) [I-type]
      // SC	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(56,regToInt(rs),regToInt(two),immToInt(offset));
  	} else if (strcmp(one, "sh") == 0) {
      // Store Halfword
      // SH rs, offset(base) [I-type]
      // SH	base	rs	offset <- machine
      char * offset = strtok(three,"()");
      char * rs = strtok(NULL,"()");
      printf("offset: %s\n", offset);
      printf("rs: %s\n", rs);
      makeI_type(41,regToInt(rs),regToInt(two),immToInt(offset));
  	} else if (strcmp(one, "sub") == 0) {
      // Subtract Word
      // SUB rd, rs, rt [R-type]
      // OP	rs	rt	rd	0	SUB <- machine
      makeR_type(0,regToInt(three),regToInt(four),regToInt(two),0,34);
  	} else if (strcmp(one, "li") == 0) { // pseudo instructions
      // Load Immidiate
      // LI rs, imm [I-type]
      //
      // lui $rs, upper( big )
      // ori $rs, $rs, lower( big )
      int imm = immToInt(three);
      makeI_type(15, 0, regToInt(two), (imm>>16));                   // lui instruction
      makeI_type(13, regToInt(two), regToInt(two), (imm & 65535));   // ori instruction

  	} else if (strcmp(one, "move") == 0) {
      // move with R-Type add
      // move $d, $s
      // add $d $s $zero
      // R[rd] = R[rs] + R[0]
      char zero[5] = "zero";
      makeR_type(0,regToInt(three),regToInt(zero),regToInt(two),0,32);
  	} else if (strcmp(one, "blt") == 0) {
      //blt $s, $t, label
      //slt $at, $rs, $rt
      //bne $at, $0, label
      char temp[4]="$at";
      char zero[5]="zero";
      makeR_type(0,regToInt(two),regToInt(three),regToInt(temp),0,42);

      int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
      makeI_type(5, regToInt(zero), regToInt(temp), (reladdr/4));

  	} else if (strcmp(one, "ble") == 0) {
      //ble $rt, $rs, LABEL
      //slt $t0, $rs, $rt
      //beq $t0, $zero, LABEL
      char temp[4]="$at";
      char zero[5]="zero";
      makeR_type(0,regToInt(three),regToInt(two),regToInt(temp),0,42);

      int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
      makeI_type(4, regToInt(zero), regToInt(temp), (reladdr/4));

  	} else if (strcmp(one, "addiu") == 0) {
        // I-type
        // addiu $t,$s,C
        // $t = $s + C (unsigned)
        // R[rt] = R[rs] + SignExtImm
        makeI_type(9, regToInt(three), regToInt(two), immToInt(four));

    } else if (strcmp(one, "addu") == 0) {
        // R-type
        // addu $d,$s,$t
        // $d = $s + $t
        // R[rd] = R[rs] + R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 33);

    } else if (strcmp(one, "andi") == 0) {
        // I-type
        // andi $t,$s,C
        // $t = $s & C
        // R[rt] = R[rs] + SignExtImm
        makeI_type(12, regToInt(three), regToInt(two), immToInt(four));

    } else if (strcmp(one, "and") == 0) {
        // R-type
        // and $d,$s,$t
        // $d = $s & $t
        // R[rd] = R[rs] & R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 36);

    } else if (strcmp(one, "beq") == 0) {
        // I-type
        // beq $s,$t,C
        // if ($s == $t) go to PC+4+4*C
        // if(R[rs]==R[rt]) PC=PC+4+BranchAddr
        int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
        makeI_type(4, regToInt(two), regToInt(three), (reladdr/4));          // relative address

    } else if (strcmp(one, "bne") == 0) {
        // I-type
        // bne $s,$t,C
        // if ($s != $t) go to PC+4+4*C
        // if(R[rs]!=R[rt]) PC=PC+4+BranchAddr
        int reladdr = labelToIntAddr(four) - (0x400000 + ((instr_index)*4)+4);
        makeI_type(5, regToInt(two), regToInt(three), (reladdr/4));          // relative address

    } else if (strcmp(one, "jal") == 0) {
        // J-type
        // jal C
        // $31 = PC + 4; PC = PC+4[31:28] . C*4
        // R[31]=PC+8;PC=JumpAddr
        makeJ_type(3, (labelToIntAddr(two)>>2));                             // absolute address

    } else if (strcmp(one, "jr") == 0) {
        // R-type
        // jr $s
        // goto address $s
        // PC=R[rs]
        makeR_type(0, regToInt(two), 0, 0, 0, 8);

    } else if (strcmp(one, "j") == 0) {
        // J-type
        // j C
        // PC = PC+4[31:28] . C*4
        // PC=JumpAddr
        makeJ_type(2, (labelToIntAddr(two)>>2));                             // absolute address
    }
    else if (strcmp(one, "lui") == 0) {
        // I-type
        // lui $t,C
        // $t = C << 16
        // R[rt] = {imm, 16�b0}
        makeI_type(15, 0, regToInt(two), immToInt(three));

    } else if (strcmp(one, "lw") == 0) {
        // I-type
        // lw $t,C($s)
        // $t = Memory[$s + C]
        // R[rt] = M[R[rs]+SignExtImm]
        char * offset = strtok(three,"()");
        char * rs = strtok(NULL,"()");
        printf("offset: %s\n", offset);
        printf("rs: %s\n", rs);
        makeI_type(35, regToInt(rs), regToInt(two), immToInt(offset));

    } else if (strcmp(one, "la") == 0) { // special
        // If the lower 16bit address is 0x0000, the ori instruction is useless.
        /*
         Case1) load address is 0x1000 0000
         lui $2, 0x1000

         Case2) load address is 0x1000 0004
         lui $2, 0x1000
         ori $2, $2, 0x0004
        */
        int location = labelToIntAddr(three);                                   // absolute label address
        if ((location & 65535) == 0) { // if lower 16bit address is 0x0000
            makeI_type(15, 0, regToInt(two), (location>>16));                   // lui instruction
        } else {
            makeI_type(15, 0, regToInt(two), (location>>16));                   // lui instruction
            makeI_type(13, regToInt(two), regToInt(two), (location & 65535));   // ori instruction
        }

    } else if (strcmp(one, "nor") == 0) {
        // R-type
        // nor $d,$s,$t
        // $d = ~ ($s | $t)
        // R[rd] = ~ (R[rs] | R[rt])
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 39);

    } else if (strcmp(one, "ori") == 0) {
        // I-type
        // ori $t,$s,C
        // $t = $s | C
        // R[rt] = R[rs] | ZeroExtImm
        makeI_type(13, regToInt(three), regToInt(two), immToInt(four));

    } else if (strcmp(one, "or") == 0) {
        // R-type
        // or $d,$s,$t
        // $d = $s | $t
        // R[rd] = R[rs] | R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 37);

    } else if (strcmp(one, "sltiu") == 0) {
        // I-type
        // sltiu $t,$s,C
        // R[rt] = (R[rs] < SignExtImm) ? 1 :0
        makeI_type(11, regToInt(three), regToInt(two), immToInt(four));

    } else if (strcmp(one, "sltu") == 0) {
        // R-type
        // sltu $d,$s,$t
        // R[rd] = (R[rs] < R[rt]) ? 1 : 0
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 43);

    } else if (strcmp(one, "sll") == 0) {
        // R-type
        // sll $d,$t,shamt
        // $d = $t << shamt
        // R[rd] = R[rt] << shamt
        makeR_type(0, 0, regToInt(three), regToInt(two), immToInt(four), 0);

    } else if (strcmp(one, "srl") == 0) {
        // R-type
        // srl $d,$t,shamt
        // $d = $t >> shamt
        // R[rd] = R[rt] >> shamt
        makeR_type(0, 0, regToInt(three), regToInt(two), immToInt(four), 2);

    } else if (strcmp(one, "sw") == 0) {
        // I-type
        // sw $t,C($s)
        // Memory[$s + C] = $t
        // M[R[rs]+SignExtImm] = R[rt]
        char * offset = strtok(three,"()");
        char * rs = strtok(NULL,"()");
        makeI_type(43, regToInt(rs), regToInt(two), immToInt(offset));

    } else if (strcmp(one, "subu") == 0) {
        // R-type
        // subu $d,$s,$t
        // $d = $s - $t
        // R[rd] = R[rs] - R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 35);
    }
}
// MIPS Fields
/*
 < R-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |   rd(5bits)  | shamt(5bits) |   fucnt(6bits)  |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

 < I-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |   rs(5bits)  |   rt(5bits)  |         constant or address (16bits)          |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

 < J-type >
  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 |    op(6bits)    |                            address (26bits)                                 |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
*/

void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= (rs << 21);
    ans |= (rt << 16);
    ans |= (rd << 11);
    ans |= (shamt << 6);
    ans |= funct;
    instructions[instr_index++] = ans;
}

void makeI_type(int op, int rs, int rt, int addr) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= (rs << 21);
    ans |= (rt << 16);
    ans |= (addr & 65535);
    instructions[instr_index++] = ans;
}

void makeJ_type(int op, int addr) {
    int32_t ans = 0;
    ans = (op << 26);
    ans |= addr;
    instructions[instr_index++] = ans;
}

// Conversion functions
int regToInt(char *reg){
	  //printf("------\nregToInt: %s\n", reg);
    char * cut = strtok(reg,"$");
    int rVal, temp;

    // cout<<"Size:"<<(int)strlen(cut)<<endl;
    int len= strlen(cut);
    // printf("Cut:%s\n",cut);

    temp = cut[1]-48;
    if(strcmp("zero", cut)==0){
    	rVal = 0;
    } else if(strcmp("gp", cut)==0){
    	rVal = 28;
    } else if(strcmp("at", cut)==0){
    	rVal = 1;
    } else if(strcmp("sp", cut)==0){
    	rVal = 29;
    } else if(strcmp("fp", cut)==0){
    	rVal = 30;
    } else if(strcmp("ra", cut)==0){
    	rVal = 31;
    } else if(cut[0] == 't' && temp>=0 && temp<8){ //temporary registers $t0-$t7
    	rVal = temp + 8;
    } else if(cut[0] == 's' && temp>=0 && temp<8){ //saved registers $s0-$s7
    	rVal = temp + 16;
    } else if (cut[0] == 't' && temp>=8 && temp<=9){ //temporary registers $t8-$t9
    	rVal = temp + 24;
    } else if(cut[0] == 'v' && temp>=0 && temp<2){
    	rVal = temp + 2;
    } else if(cut[0] == 'a' && temp>=0 && temp<4){
    	rVal = temp + 4;
    } else{
    	rVal = -1;
    }
    // printf("rVal: %d\n", rVal);
    return rVal;
}

int labelToIntAddr(char *label) {
    int i;
    for (i=0; i<labelindex; i++) {
        if (strcmp(label_list[i].name, label) == 0) {
            return label_list[i].location;
        }
    }
    return -1;
}

int immToInt(char *immediate){
    // 2 cases - hexadecimal and decimal.
    if (strstr(immediate,"0x") != NULL) {
        return (int)strtol(immediate, NULL, 0);
    } else {
        return atoi(immediate);
    }
}

char *intTobin(int input) {
    string str = bitset<32>(input).to_string();
    char *writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';
    cout << writable << "\n";
    return writable;
}

void showInstructions() {
    for (int i=0; i < instr_index; i++ ) {
        string binary = bitset<32>(instructions[i]).to_string(); //to binary
        cout<<binary<<"\n";
    }
}
