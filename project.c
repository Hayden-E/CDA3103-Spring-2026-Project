#include "spimcore.h"


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch(ALUControl) 
    {
        case 000:
            *ALUresult = (A+B);
            break;
        
        case 001:
            *ALUresult = (A-B);
            break;

        case 010:
            *ALUresult = A < B;
            break;

        //?
        case 011:
            *ALUresult = A < B;
            break;

        case 100:
            *ALUresult = A && B;
            break;

        case 101:
            *ALUresult = A || B;
            break;

        case 110:
            *ALUresult = B<<16;
            break;

        case 111:
            *ALUresult = ~A;
            break;
    }

    *Zero = (*ALUresult == 0);
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
//this will read the registers specified by r1 and r2 and store the values in data1 and data 2. Note that the register file is an array of 32 unsigned integers called Reg.
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */

//read memory put ionto register 
//write register value into mem
//aluresults = address
//moves the bit by 2 converts byte address to word index
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

    //check to see if ALU result is a valid address(only if it is a address.

    if(ALUresult % 4 != 0)return 1;

    //Load word 
    if(MemRead == 0x1 && MemWrite == 0x0)
    {
        *memdata = Mem[ALUresult >> 2];
    } 

    //store word into memory
    if(MemRead == 0x0 && MemWrite == 0x1)
    {
        Mem[ALUresult >> 2] = data2;
    }

    return 0;

}



/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{

}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

