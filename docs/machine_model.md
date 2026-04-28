# Machine model
Zorita is a 16-bit RISC machine consisting of:
- a memory,
- a set of registers,
- an instruction decoder,
- an instruction execution unit, and
- a condition checker.

![Machine model](machine_model.png)

## Memory
Memory size is 64K words, each being 16-bit wide.  
The whole memory is addressable with a 16-bit register.

## Registers  
All registers are also 16-bit wide.

### Data registers
Data registers are used as general-purpose registers.  
There are 16 data registers: *R0* through *R15*.

### Instruction pointer
The instruction pointer (*IP*) holds the address of the next instruction to be executed.  

### Status register
The status register (*ST*) holds a set of flags that provide information about the result of the last operation.  
The only operations that update the status register are CMP, ADD, and SUB.
- *ZF*: result was zero.
- *NF*: result was negative.
- *CF*: operation produced a carry.
- *OF*: operation produced an overflow.

## Memory image of a program  
The memory image of a program is the layout of a binary program once it has been loaded into memory.  
There are only 2 sections:
- An initialized DATA section, useful to allocate predefined values.
- A CODE section, where the actual machine code is stored.

### DATA section
The DATA section starts at memory position 0.
Memory position 0 should be initialized to the address of the first instruction.
That is, the address where the CODE section starts.
The rest of the DATA section could be used to store any other predefined values.
For example, considering the limitations of the instruction set, a target address for a JMP instruction.

### CODE section  
The CODE section can start anywhere.
For example, at the next address after the DATA section.
The address where the CODE section starts should be stored at memory position 0.

## Signedness   
Zorita is sign-agnostic.
That is, registers and memory positions are just treated as 16-bit strings.
CMP, ADD, and SUB operations update the status register flags.
The interpretation of those flags is then left to the programmer.

## Endianness  
Endianness is not applicable.
Since each memory position is 16 bits wide,
and all data types (operands, addresses, and instructions) fit in a single 16-bit word,
no multi-word storage is needed.
