# Instruction set
Instructions are 16-bit long, where bit 15 is the most significant bit.
In the tables below, the most significant bits are shown leftmost.

Instruction opcode is always encoded as bits 15:13.  
Register operands are encoded as 4-bit values ranging from 0000 (*R0*) to 1111 (*R15*).  

## HALT
Stop the machine.

### Machine code
| 15:13 | 12:0 |
| :---: | :---: |
| 000 | — |

## CMP *op1*, *op2*
Where *op1* and *op2* are data registers.

Perform *op1* - *op2*, and update *ZF*, *NF*, *CF*, and *OF* flags.
Result of subtraction is not stored.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:0 |
| :---: | :---: | :---: | :---: |
| 001 | *op1* | *op2* | — |

## JMP *cond*, *addr*
Where *cond* is a condition, and *addr* is a data register.  
The condition is encoded as a 4-bit value, as shown in the table below.

| Code | Condition     | Logic        | Code | Condition            | Logic        |
|:----:|:--------------|:-------------|:----:|:---------------------|:-------------|
| 0000 | Zero (Equal)  | ZF=1         | 1000 | Non-zero (Not equal) | ZF=0         |
| 0001 | Negative      | NF=1         | 1001 | Not negative         | NF=0         |
| 0010 | Carry         | CF=1         | 1010 | Not carry            | CF=0         |
| 0011 | Overflow      | OF=1         | 1011 | Not overflow         | OF=0         |
| 0100 | Less than     | NF≠OF        | 1100 | Greater or equal     | NF=OF        |
| 0101 | Less or equal | (NF≠OF)∨ZF=1 | 1101 | Greater than         | (NF=OF)∧ZF=0 |
| 0110 | —             | —            | 1110 | —                    | —            |
| 0111 | Always        | —            | 1111 | Never                | —            |

If a given condition *cond* is met,
reset the instruction pointer *IP* to the memory position stored in *addr*.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:0 |
| :---: | :---: | :---: | :---: |
| 010 | *cond* | *addr* | — |

## LOAD *reg*, *addr*
Where *reg* and *addr* are data registers.

Load register *reg* with the contents of the memory position pointed by *addr*.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:0 |
| :---: | :---: | :---: | :---: |
| 011 | *reg* | *addr* | — |

## STORE *reg*, *addr*
Where *reg* and *addr* are data registers.

Store the value of register *reg* in the memory position pointed by *addr*.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:0 |
| :---: | :---: | :---: | :---: |
| 100 | *reg* | *addr* | — |

## ADD *dst*, *src1*, *src2*
Where *dst*, *src1*, and *src2* are data registers.

Add *src1* and *src2* and leave the result in *dst*.
*ZF*, *NF*, *CF*, and *OF* flags are updated.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:1 | 0 |
| :---: | :---: | :---: | :---: | :---: |
| 101 | *dst* | *src1* | *src2* | — |

## SUB *dst*, *src1*, *src2*
Where *dst*, *src1*, and *src2* are data registers.

Subtract *src2* from *src1* and leave the result in *dst*.
*ZF*, *NF*, *CF*, and *OF* flags are updated.

### Machine code
| 15:13 | 12:9 | 8:5 | 4:1 | 0 |
| :---: | :---: | :---: | :---: | :---: |
| 110 | *dst* | *src1* | *src2* | — |

## Invalid instruction
Any other instruction will be considered invalid.
For this instruction set, that can only happen if the instruction opcode is *111*.

Stop the machine with an error.

### Machine code
| 15:13 | 12:0 |
| :---: | :---: |
| 111 | — |
