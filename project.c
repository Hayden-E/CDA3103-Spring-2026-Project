// CDA 3103C-26
// Sarah Angell
// April 24th 2026
// Developed by Tyler Mazurek, and Elizabeth Schmidt 
//hayden didnt do anything

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
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

    // Set zero value
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
    // initialize
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    switch(op)
    {
        case 0x0: // R type (add, sub, and, or, slt, sltu)
        controls->RegDst = 1;
        controls->ALUOp = 7; // 111 in binary (R type)
        controls->RegWrite = 1;
        break;

        case 0x2: // jump
        controls->RegDst = 2;
        controls->Jump = 1;
        controls->MemtoReg = 2;
        controls->Branch = 2;
        controls->ALUSrc = 2; 
        break;

        case 0x4: // beq
        controls->RegDst = 2;
        controls->Branch = 1;
        controls->MemtoReg = 2;
        controls->ALUOp = 1; // subtraction needed to determine if equal
        break;

        case 0x8: // addi
        controls->RegWrite = 1;
        controls->ALUSrc = 1;
        break;

        case 0xa: // slti
        controls->ALUOp = 2;
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;

        case 0xb: // sltiu
        controls->ALUOp = 3; // slt unsigned
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;

        case 0xf: // lw upper immediate
        controls->ALUOp = 6; // need upper 16 bits so SHIFT!
        controls->ALUSrc = 1;
        controls->RegWrite = 1;
        break;

        case 0x23: // lw
        controls->RegWrite = 1;
        controls->MemtoReg = 1;
        controls->MemRead = 1;
        controls->ALUSrc = 1;
        break;

        case 0x2b: // sw
        controls->RegDst = 2;
        controls->MemtoReg = 2;
        controls->MemWrite = 1;
        controls->ALUSrc = 1;
        break;

        default: // halt
            return 1;
    }
    return 0; // no halt
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
    if((offset >> 15) == 1)
    {
        *extended_value = offset | 0xffff0000; // first 16 bits become 1's
    }
    else
    {
        *extended_value = offset & 0x0000ffff; // first 16 bits become 0's
    }
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    int ALUControl;

    if(ALUOp < 0 || ALUOp > 7)
    {
        return 1; // halt b/c out of bounds
    }

    if(ALUOp == 7) // R-Type
    {
        switch(funct)
        {
            case 0x20: // add
                ALUControl = 0;
            break;
            
            case 0x22: // sub
            ALUControl = 1;
            break;

            case 0x24: // and
                ALUControl = 4;
            break;

            case 0x25: // or
                ALUControl = 5;
            break;

            case 0x2a: // slt
                ALUControl = 2;
            break;

            case 0x2b: // sltu
                ALUControl = 3;
            break;

            default:
                return 1; // halt
        }
        ALU(data1, data2, ALUControl, ALUresult, Zero);
    }
    else
    {
        if(ALUSrc == 1)
        {
            ALU(data1, extended_value, ALUOp, ALUresult, Zero);
        }
        else
        {
            ALU(data1, data2, ALUOp, ALUresult, Zero);
        }
    }
    return 0;
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
    //check if we are writing to a register
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
    unsigned nextPC = *PC + 4;

    //we can jump if the instruction is a jump or if the instruction is a branch 
    if (Jump == 1)
    {
        *PC = (nextPC & 0xF0000000) | (jsec << 2);
    }
    else if (Branch == 1 && Zero == 1)
    {
        *PC = nextPC + (extended_value << 2);
    }
    else
    {
        *PC = nextPC;
    }
}