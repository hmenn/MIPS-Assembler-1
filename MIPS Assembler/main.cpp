//
//  main.cpp
//  MIPS Assembler
//
//  Created by Young Seok Kim
//  Copyright © 2015 TonyKim. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <string.h>
using namespace std;

// data structures
struct label {
    char name;
    int32_t location;
};

// Global variables
label lists[100];
int labelindex = 0;

int32_t instructions[1000];
int instr_index = 0;


// functions
void assembleLine(char *line);

void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct);
void makeI_type(int op, int rs, int rt, int addr);
void makeJ_type(int op, int addr);

int regToInt(char *reg);
int labelToIntAddr(char *label);
int immToInt(char *immediate);



/* MAIN */


int main(int argc, const char * argv[]) {
    // insert code here...
    
    if (argv[1] == NULL) {
        printf("Please type in input file name.\n");
    }
    
    string line;
    ifstream inputfile(argv[1]);
    if (inputfile.is_open())
    {
        
        
        
        
        
        
        
        while ( getline(inputfile,line) )
        {
            cout << line << '\n';
        }
        inputfile.close();
    }
    else cout << "Unable to open file";
    
    
    
    
    //std::cout << "Hello, World!\n";
    return 0;
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
            }
        }
        inputfile.close();
    }
    else cout << "Unable to open file";
    
    return count;
}

void assembleLine(char *line){
    
    
    char key[] = " ,";
    
    char * one;
    char * two;
    char * three;
    char * four;
    
    one = strtok(line,key);
    
    if(one != NULL)
        two = strtok(NULL,key);
    if(two != NULL)
        three = strtok(NULL,key);
    if(three != NULL)
        four = strtok(NULL,key);
    
    
    if (strcmp(one, "addiu") == 0) {
        // I-type
        // addiu $t,$s,C
        // $t = $s + C (unsigned)
        // R[rt] = R[rs] + SignExtImm
        makeI_type(9, regToInt(three), regToInt(two), atoi(four));
        
    } else if (strcmp(one, "addu") == 0) {
        // R-type
        // addu $d,$s,$t
        // $d = $s + $t
        // R[rd] = R[rs] + R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 33);
        
    } else if (strcmp(one, "addi") == 0) {
        // I-type
        // addi $t,$s,C
        // $t = $s + C (signed)
        // R[rt] = R[rs] + SignExtImm
        makeI_type(8, regToInt(three), regToInt(two), atoi(four));
        
    } else if (strcmp(one, "add") == 0) {
        // R-type
        // add $d,$s,$t
        // $d = $s + $t
        // R[rd] = R[rs] + R[rt]
        makeR_type(0, regToInt(three), regToInt(four), regToInt(two), 0, 32);
        
    } else if (strcmp(one, "beq") == 0) {
        // I-type
        // beq $s,$t,C
        // if ($s == $t) go to PC+4+4*C
        // if(R[rs]==R[rt]) PC=PC+4+BranchAddr
        makeI_type(4, regToInt(two), regToInt(three), atoi(four));
        
    } else if (strcmp(one, "bne") == 0) {
        // I-type
        // bne $s,$t,C
        // if ($s != $t) go to PC+4+4*C
        // if(R[rs]!=R[rt]) PC=PC+4+BranchAddr
        makeI_type(5, regToInt(two), regToInt(three), atoi(four));
        
    } else if (strcmp(one, "jal") == 0) {
        // J-type
        // jal C
        // $31 = PC + 4; PC = PC+4[31:28] . C*4
        // R[31]=PC+8;PC=JumpAddr
        makeJ_type(3, atoi(two));
        
    } else if (strcmp(one, "jr") == 0) {
        // R-type
        // jr $s
        // goto address $s
        // PC=R[rs]
        makeR_type(0, regToInt(two), 0, 0, 0, 8);
        
    } else if (strcmp(one, "lui") == 0) {
        // I-type
        // lui $t,C
        // $t = C << 16
        // R[rt] = {imm, 16’b0}
        makeI_type(15, 0, regToInt(two), immToInt(three));
        
    } else if (strcmp(one, "lw") == 0) {
        // I-type
        // lw $t,C($s)
        // $t = Memory[$s + C]
        // R[rt] = M[R[rs]+SignExtImm]
        char * offset = strtok(three,"()");
        char * rs = strtok(NULL,"()");
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
        int location = labelToIntAddr(three);
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
    return
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
 |    op(6bits)    |                            address (30bits)                                 |
 +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 
*/


void makeR_type(int op, int rs, int rt, int rd, int shamt, int funct) {
    int32_t ans;
    ans = (op << 26);
    ans &= (rs << 21);
    ans &= (rt << 16);
    ans &= (rd << 11);
    ans &= (shamt << 6);
    ans &= funct;
    instructions[instr_index++] = ans;
}

void makeI_type(int op, int rs, int rt, int addr) {
    int32_t ans;
    ans = (op << 26);
    ans &= (rs << 21);
    ans &= (rt << 16);
    ans &= addr;
    instructions[instr_index++] = ans;
}

void makeJ_type(int op, int addr) {
    int32_t ans;
    ans = (op << 26);
    ans &= addr;
    instructions[instr_index++] = ans;
}





//

int regToInt(char *reg){
    //$ 빼고 int로 바꾸기.
}

int labelToIntAddr(char *label) {
    
}

int immToInt(char *immediate){
    // 2 cases.
    // hex, decimal
    
}


// 1. Find Jump Locations
// 2. Make instructions (funciton)

























