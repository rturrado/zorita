# Code design
Zorita doesn't build as an executable.
It just builds as a library which is then exercised via the tests.

Zorita library offers a `Machine` class, consisting mainly of a `Memory` and some `Registers`:
general-purpose data registers, an instruction pointer, and a status register.

A machine can be initialized with a binary program by correctly setting the memory and registers.
For example, the registers can contain constants, code addresses, and variables;
and the memory, predefined data in the DATA section and the code.
See [Code examples](code_examples.md) for a couple of sample test programs.

Once initialized, the program can be simulated by invoking the `Machine::run()` method.

The simulation can have the following outcomes:
1. The machine runs until it executes a `halt` instruction and returns to `Stopped` state.
2. The machine tries to execute an invalid instruction and throws an exception.
3. The machine loops indefinitely.

## Class diagram
The `Memory` class is implemented as an array of 64K `uint16_t` elements.
The data registers, `Registers::rx_`, are also defined as an array of 16 `uint16_t`.
And the `StatusRegister` uses internally a bitset of size 16.

`Instruction` is implemented as a `variant`,
given that all types of instructions are known at compile time.
`Machine` overloads the `execute()` method for every type of instruction.
And `Machine::execute()` invokes each overload by visiting the `Instruction` parameter.

```mermaid                                                                                                                                                                                                                                                                                                    
  classDiagram                                                                                                                                                                                                                                                                                                  
      class Machine {                                                                                                                                                                                                                                                                                           
          -Memory memory_                                                                                                                                                                                                                                                                                       
          -Registers registers_
          -State state_
          +Machine()
          +Machine(const std::vector<uint16_t> &)
          +Machine(const std::vector<uint16_t> &, const Registers &)
          +run()                                                                                                                                                                                                                                                                                              
          +step()
          +execute(const Instruction &)
      }
    class Memory {
        -std::array~uint16_t, 65536~ data_
        +Memory(const std::vector<uint16_t> &)
        +read(uint16_t) uint16_t
        +write(uint16_t, uint16_t)
    }
    class Registers {
          -array~uint16_t, 16~ rx_
          -uint16_t ip_                                                                                                                                                                                                                                                                                         
          -StatusRegister st_
          +Registers(const std::vector<uint16_t> &)
      }                                                                                                                                                                                                                                                                                                         
      class StatusRegister {
          -std::bitset~16~ data_
          +StatusRegister(uint16_t)
          +zf() bool
          +nf() bool
          +cf() bool
          +of() bool
          +to_uint16() uint16_t
          +set_st(uint16_t)
          +set_flag(Flag, bool)
      }
      class Decoder {
          +decode(uint16_t)$ Instruction                                                                                                                                                                                                                                                                        
      }                                                                                                                                                                                                                                                                                                       
      class Instruction {
          <<variant>>
          Halt
          | Cmp
          | Jmp
          | Load
          | Store
          | Add
          | Sub                                                                                                                                                                                                                                                                              
      }
                                                                                                                                                                                                                                                                                                                
      Machine *-- Memory                                                                                                                                                                                                                                                                                      
      Machine *-- Registers
      Registers *-- StatusRegister
      Machine ..> Decoder : uses
      Decoder ..> Instruction : produces
      Machine ..> Instruction : executes                                                                                                                                                                                                                                                                        
  ```

## Enumeration diagram
Enumerations are used with the intention of:
- expressing the meaning of some values more clearly, and
- helping to enforce strong typing.

The following enumerations are defined:
- `State` holds the current state of the machine.
- `Flag` enumerates the available flags in the status register.
- `ConditionOpcode` maps every jump condition to its opcode (e.g., `Always` to `0b111`).

```mermaid
  classDiagram
      class State {
          <<enumeration>>
          Stopped
          Running
      }
      class Flag {
          <<enumeration>>
          Zero
          Negative
          Carry
          Overflow
      }
      class ConditionOpcode {
          <<enumeration>>
          Zero
          Negative
          Carry
          Overflow
          LessThan
          LessOrEqual
          Always
          NotZero
          NotNegative
          NotCarry
          NotOverflow
          GreaterOrEqual
          GreaterThan
          Never
      }
  ```

## State diagram
The machine can only be in two states: `Stopped`, or `Running`:  
- Machine constructors initialize the state as `Stopped`.  
- `Machine::run()` changes state to `Running`.
- `Machine::step()` merely invokes `Machine::execute()` and does not change the state.
- `Machine::execute()` can then:
  - Change the state back to `Stopped` for a `Halt` instruction.
  - Throw an exception if the instruction is invalid.
  - Otherwise, leave the state unchanged.

```mermaid                                                                                                                                                                                                                                                                                                    
  stateDiagram                                                                                                                                                                                                                                                                                             
      [*] --> Stopped
      Stopped --> Running : run()                                                                                                                                                                                                                                                                               
      Running --> Running : execute()
      Running --> [*] : execute(Halt)
      Running --> [*] : execute(Invalid)                                                                                                                                                                                                                                                                                
  ```