# Code examples
The code examples below implement two functions in the assembler of the Zorita machine. 

## Fibonacci
The Fibonacci example calculates `fibonacci(n)`.  
`n` is passed through memory, and the result is stored in a register.  
The biggest number that can be computed in this machine is `fibonacci(24)`.
For `n` bigger than 24, the Fibonacci number cannot be encoded as an `uint16_t`.  
The program uses a circular buffer to store the last two Fibonacci numbers.

- If `n` is either 0 or 1, the program returns quickly.
- Otherwise, the program loops until the `i` counter is equal to `n`.  
  For every iteration, up to two new Fibonacci numbers are calculated
  (just by adding the numbers in the circular buffer)
  and stored into `lf[0]` and `lf[1]`, respectively.  
  If the addition produces a carry, the program terminates with an error.
- The result is always computed as `fibonacci(n)`.

### Registers

| Register | Value  | Description              |
|----------|--------|--------------------------|
| R0       | 0x0000 | Constant 0               |
| R1       | 0x0001 | Constant 1               |
| R2       | 0x0001 | Address of `n`, then `n` |
| R3       | 0x0008 | Address of `loop` label  |
| R4       | 0x0013 | Address of `ret` label   |
| R5       | 0x0016 | Address of `err` label   |
| R6       | 0x0001 | `i`                      |
| R7       | 0x0000 | `lf[0]`                  |
| R8       | 0x0000 | `lf[1]`                  |
| R9       | 0x0000 | `res`                    |

### Memory

### DATA section

| Address | Value  | Description             |
|---------|--------|-------------------------|
| 0x0000  | 0x0002 | Address of CODE section |
| 0x0001  | 0x0024 | `n`                     |

### CODE section

| Address | Value  | Label | Mnemonic                | Comment           |
|---------|--------|-------|-------------------------|-------------------|
| 0x0002  | 0x0000 |       | load n, n               | n = *n            |
| 0x0003  | 0x2400 |       | cmp n, 0                |                   |
| 0x0004  | 0x4080 |       | je ret                  | jmp ret if n == 0 |
| 0x0005  | 0xb002 |       | add lf[1], 0, 1         | lf[1] = 1         |
| 0x0006  | 0x2420 |       | cmp n, 1                |                   |
| 0x0007  | 0x4080 |       | je ret                  | jmp ret if n == 1 |
| 0x0008  | 0xacc2 | loop: | add i, i, 1             | i++               |
| 0x0009  | 0x24c0 |       | cmp n, i                |                   |
| 0x000a  | 0x4080 |       | je ret                  | jmp ret if n == i |
| 0x000b  | 0xaef0 |       | add lf[0], lf[0], lf[1] | lf[0] = fib(i)    |
| 0x000c  | 0x44a0 |       | jc err                  | jmp err if carry  |
| 0x000d  | 0xacc2 |       | add i, i, 1             | i++               |
| 0x000e  | 0x24c0 |       | cmp n, i                |                   |
| 0x000f  | 0x4080 |       | je ret                  | jmp ret if n == i |
| 0x0010  | 0xb0f0 |       | add lf[1], lf[0], lf[1] | lf[1] = fib(i)    |
| 0x0011  | 0x44a0 |       | jc err                  | jmp err if carry  |
| 0x0012  | 0x4e60 |       | j loop                  |                   |
| 0x0013  | 0xb0f0 | ret:  | add res, lf[0], lf[1]   | res = fib(n)      |
| 0x0014  | 0x44a0 |       | jc err                  | jmp err if carry  |
| 0x0015  | 0x0000 |       | halt                    |                   |
| 0x0016  |        | err:  |                         |                   |

## Accumulator
The Accumulator example sums a list of `int16_t` values.  
The `size` of the list is passed through memory, and the result is stored in a register.  
The list can have a maximum of 10 elements.

- If `size` is either 0 or greater than 10, the program returns quickly
  (with a result of 0 or an error, respectively).
- Otherwise, the program loops until the `i` counter is equal to `size`.  
  For every iteration, the next value `val` is loaded from the memory pointed by `vptr`
  and added to an accumulator `acc`.  
  If the sum produces a signed overflow, the program terminates with an error.

### Registers

| Register | Value  | Description                         |
|----------|--------|-------------------------------------|
| R0       | 0x0000 | Constant 0                          |
| R1       | 0x0001 | Constant 1                          |
| R2       | 0x000a | Constant 10                         |
| R3       | 0x0001 | Address of `size`, then `size`      |
| R4       | 0x0002 | Address of `values[0]`, then `vptr` |
| R5       | 0x0011 | Address of `loop` label             |
| R6       | 0x0019 | Address of `ret` label              |
| R7       | 0x001a | Address of `err` label              |
| R8       | 0x0001 | `i`                                 |
| R9       | 0x0000 | `val`                               |
| R10      | 0x0000 | `acc`                               |

### Memory

### DATA section

| Address | Value  | Description             |
|---------|--------|-------------------------|
| 0x0000  | 0x000c | Address of CODE section |
| 0x0001  | 0x0004 | `size`                  |
| 0x0002  | 0x1000 | `values[0]`             |
| 0x0003  | 0x2000 | `values[1]`             |
| 0x0004  | 0x4000 | `values[2]`             |
| 0x0005  | 0x7fff | `values[3]`             |
| 0x0006  | 0x0000 | `values[4]`             |
| 0x0007  | 0x0000 | `values[5]`             |
| 0x0008  | 0x0000 | `values[6]`             |
| 0x0009  | 0x0000 | `values[7]`             |
| 0x000a  | 0x0000 | `values[8]`             |
| 0x000b  | 0x0000 | `values[9]`             |

### CODE section

| Address | Value  | Label | Mnemonic              | Comment                    |
|---------|--------|-------|-----------------------|----------------------------|
| 0x000c  | 0x6660 |       | load size, size       | size = *size               |
| 0x000d  | 0x2640 |       | cmp size, 10          |                            |
| 0x000e  | 0x5ae0 |       | jg err                | jmp err if size > 10       |
| 0x000f  | 0x2600 |       | cmp size, 0           |                            |
| 0x0010  | 0x40c0 |       | je ret                | jmp ret if size == 0       |
| 0x0011  | 0x7280 | loop: | load val, vptr        | val = *vptr                |
| 0x0012  | 0xb552 |       | add acc, acc, val     | acc += val                 |
| 0x0013  | 0x46e0 |       | jv err                | jmp err if overflow        |
| 0x0014  | 0x3060 |       | cmp i, size           |                            |
| 0x0015  | 0x40c0 |       | je ret                | jmp ret if i == size       |
| 0x0016  | 0xb102 |       | add i, i, 1           | i++                        |
| 0x0017  | 0xa882 |       | add vptr, vptr, 1     | vptr++                     |
| 0x0018  | 0x4ea0 |       | j loop                |                            |
| 0x0019  | 0x0000 | ret:  | halt                  |                            |
| 0x001a  |        | err:  |                       |                            |
