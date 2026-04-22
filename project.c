#include "spimcore.h"


/* ALU */
/* 10 Points */
//hello hayden
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    //cooked
    switch(ALUControl) 
    {
        case 0x0: // A + B
            *ALUresult = (A+B);
            break;
        
        case 0x1: // A - B
            *ALUresult = (A-B);
            break;

        case 0x2: // A < B (signed)
            *ALUresult = A < B;

            unsigned mask = 0x10000000;
            unsigned f = 0x00000000;
            unsigned Aneg = mask & A;
            unsigned Bneg = mask & B;

            if(Aneg == f && Bneg == f)
            {
                // A and B are positive

                if(A < B)
                    *ALUresult = 0x1;
                else
                    *ALUresult = 0x0;
            }
            else if(Aneg == f && Bneg == mask) 
            {
                // A is positive B is negative

                *ALUresult = 0x0;
            }
            else if(Aneg == mask && Bneg == f) 
            {
                // A is negative B is positive

                *ALUresult = 0x1;
            }
            else 
            {
                // A and B are negative
                
                if((~A + 1) < (~B + 1))
                    *ALUresult = 0x1;
                else
                    *ALUresult = 0x0;
            }

            break;

        case 0x3: // A <  B (unsigned)
            *ALUresult = A < B;
            break;

        case 0x4: // A and B
            *ALUresult = A & B;
            break;

        case 0x5: // A or B
            *ALUresult = A | B;
            break;

        case 0x6: // Shift B left 16 bits
            *ALUresult = B<<16;
            break;

        case 0x7: // Not A
            *ALUresult = ~A;
            break;
    }

    if(*ALUresult == 0x0)
        *Zero = 0x1;
    else
        *Zero = 0x0;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // Check that PC is a multiple of 4
    if(PC % 4 != 0x0)
        return 1;

    // Make sure PC is in appropriate range
    if(PC < 0x0000 || PC > 0xFFFF)
        return 1;

    // Fetch instruction from mem
    *instruction = Mem[PC>>2];

    // Success
    return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // Extracting bits from instruction

    // Bits 31 - 26
    *op = instruction & (0x3F<<26);
    // Bits 25 - 21
    *r1 = instruction & (0x1F<<21);
    // Bits 20 - 16
    *r2 = instruction & (0x1F<<16);
    // Bits 15 - 11
    *r3 = instruction & (0x1F<<11);
    // Bits 5 - 0
    *funct = instruction & (0x3F);
    // Bits 15 - 0
    *offset = instruction & (0xFFFF);
    // Bits 25 - 0
    *jsec = instruction & (0x3FFFFFF);
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // Ella
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
    // Ella
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // Ella
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

    if(MemRead == 1)
    {
        if(ALUresult % 4 != 0)
        {
            return 1;
        }

        *memdata = Mem[ALUresult >> 2];
    }

    if(MemWrite == 1)
    {
        if(ALUresult % 4 != 0)
        {
            return 1;
        }

        Mem[ALUresult >> 2] = data2;
    }
    return 0;

}



/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if(RegWrite)
    {
        unsigned dest;
        if(RegDst)dest = r3;  //r-type
        else dest = r2; //i-type 

        if(dest == 0)return;

        if(MemtoReg)
        {
            Reg[dest] = memdata; //lw
        }
        else Reg[dest] = ALUresult; //r-type addi lui
    }
}



/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //for next instruction
    if(Branch == 0x0 && Jump == 0x0)
    {
        *PC = *PC + 4; //go to next instruction
    }

    //branch equal
    if(Branch == 0x1 && Jump ==0x0 && Zero == 0x1)
    { 
        //This is my comment
        *PC = (extended_value << 2) + (*PC + 4);

    }

    //then we need to jump
    if(Branch == 0x0 && Jump == 0x1)
    {
        //shift to left 2 bits
        unsigned jsecShift = jsec << 2;
       //will figure out 
        unsigned PC4Bits = (*PC + 4) & 0xF0000000;//bitmask for PC + 4 first 4 bits 
        *PC = PC4Bits | jsecShift;//combine bits with 28 bit jsec
    }

}

