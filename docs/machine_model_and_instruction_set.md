# Machine model

**How big is the memory?**  
64K positions.

**How wide is each memory position?**  
16 bits. That is, we can address 64K positions with a 16-bit register.

**How big are the registers?**  
16 bits.

**What data registers do we have?**
- *R0*, *R1*, *R2*, *R3*, *R4*, *R5*, *R6*, *R7*: generic data registers.
- *IP*: instruction pointer.

**What status registers do we have?**
- *ST*.

**What flags do we have in the status register?**
- *ZF*: result of CMP, ADD, or SUB operations was zero.
- *NF*: result of CMP, ADD, or SUB operations was negative.
- *OF*: CMP, ADD, or SUB operations overflowed.
  - pos + pos > MAXINT.
  - neg + neg < MININT.
  - pos - neg > MAXINT.
  - neg - pos < MININT.

**How does the memory image of a process look like?**  
We have an initialized DATA section, e.g., for immediate values, and then a CODE section.  
We don't have static variables, so we don't need a BSS.  
We don't have CALL instructions, so we don't need a STACK.  
We don't have dynamic allocation, so we don't need a HEAP.

**Where does the CODE section start?**  
CODE could start anywhere.
We are just going to set it to the first free position after the DATA section,
but we are not going to enforce this DATA before CODE by code.

> **Note:**
> *IP* should be initialized to where CODE starts.

**What primitive data types do we have?**   
Addresses are treated as unsigned integers, i.e., in JMP, LOAD, and STORE.  
Operands are treated as signed integers in CMP, ADD, and SUB, and as unsigned integers in LOAD and STORE.

**What representation do we use for signed integers?**  
Two's complement.
If we implement this RISC machine in C++20, we can just use *int16_t*,
because the standard guarantees that signed integers are represented in two's complement.

> **Note:**
> MAXINT = 2<sup>15</sup> - 1, and MININT = -2<sup>15</sup>.

**What is the endianness of the machine?**  
Not applicable.
Since each memory position is 16 bits wide,
and all data types (operands, addresses, and instructions) fit in a single 16-bit word,
no multi-word storage is needed.

# Instruction set

Instructions are 16-bit long, where bit 15 is the most significant bit.     
Instruction opcode is encoded as bits 15:13.  
Register operands are encoded as 3-bit values where 0b000 = *R0* and 0b111 = *R7*.  
Flags are encoded as 2-bit values where 0b00 = *ZF*, 0b01 = *NF*, and 0b10 = *OF*.

## HALT
Stop the machine.

### Machine code
| 15:13 | 12:0 |
| :---: | :---: |
| 000 | — |

## CMP *op1*, *op2*
Where *op1* and *op2* are data registers.

Perform *op1* - *op2*, and update *ZF*, *NF*, and *OF* flags.
Result of subtraction is not stored.

### Machine code
| 15:13 | 12:10 | 9:7 | 6:0 |
| :---: | :---: | :---: | :---: |
| 001 | *op1* | *op2* | — |

## JMP (*flag*,) *addr*
Where *addr* is a data register, and *flag* is an optional flag from the status register.

Update *IP* to *addr*, optionally conditioned on whether a given flag is set.

### Machine code
Bit 12 is the conditional jump bit. 1 if jump is conditional, 0 otherwise.  

| 15:13 |  12  | 11:10 | 9:7 | 6:0 |
| :---: |:----:| :---: | :---: | :---: |
| 010 | *cj* | *flag* | *addr* | — |

## LOAD *reg*, *addr*
Where *reg* and *addr* are data registers.

Load register *reg* with the contents of the memory position pointed by *addr*.

### Machine code
| 15:13 | 12:10 | 9:7 | 6:0 |
| :---: | :---: | :---: | :---: |
| 011 | *reg* | *addr* | — |

## STORE *reg*, *addr*
Where *reg* and *addr* are data registers.

Store the value of register *reg* in the memory position pointed by *addr*.

### Machine code
| 15:13 | 12:10 | 9:7 | 6:0 |
| :---: | :---: | :---: | :---: |
| 100 | *reg* | *addr* | — |

## ADD *dst*, *src1*, *src2*
Where *dst*, *src1*, and *src2* are data registers.

Add *src1* and *src2* and leave the result in *dst*.
*ZF*, *NF*, and *OF* flags are updated.

### Machine code
| 15:13 | 12:10 | 9:7 | 6:4 | 3:0 |
| :---: | :---: | :---: | :---: | :---: |
| 101 | *dst* | *src1* | *src2* | — |

## SUB *dst*, *src1*, *src2*
Where *dst*, *src1*, and *src2* are data registers.

Subtract *src2* from *src1* and leave the result in *dst*.
*ZF*, *NF*, and *OF* flags are updated.

### Machine code
| 15:13 | 12:10 | 9:7 | 6:4 | 3:0 |
| :---: | :---: | :---: | :---: | :---: |
| 110 | *dst* | *src1* | *src2* | — |