// Advent of Code 2018
// Day 16: Chronal Classification
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
using namespace std;

// Define the device to simulate as per the given specs.
struct Device {
    typedef array<uintmax_t, 4> Registers;   // 4 registers set [0, 1, 2, 3]
    typedef array<uintmax_t, 4> Instruction; // [OP, Ain, Bin, Cout]
    typedef void (*Op)(Registers &, size_t, size_t, size_t); // fn pointer

    Registers regs; // set of 4 registers in the device

    // opcode template: register vs. immediate "specialization" via bool flags
    template <typename F, bool RegA = true, bool RegB = true>
    static void OPR(Registers &regs, size_t A, size_t B, size_t C) {
        F function; // the operation to execute
        regs[C] = function(RegA ? regs[A] : A, RegB ? regs[B] : B); // execute
    }

    // the opcode definitions
    constexpr static array<Op, 16> Opcodes = {{
        OPR<plus<size_t>, true, true>,        // addr
        OPR<plus<size_t>, true, false>,       // addi
        OPR<multiplies<size_t>, true, true>,  // mulr
        OPR<multiplies<size_t>, true, false>, // muli
        OPR<bit_and<size_t>, true, true>,     // banr
        OPR<bit_and<size_t>, true, false>,    // bani
        OPR<bit_or<size_t>, true, true>,      // borr
        OPR<bit_or<size_t>, true, false>,     // bori
        OPR<greater<size_t>, false, true>,    // gtir
        OPR<greater<size_t>, true, false>,    // gtri
        OPR<greater<size_t>, true, true>,     // gtrr
        OPR<equal_to<size_t>, false, true>,   // eqir
        OPR<equal_to<size_t>, true, false>,   // eqri
        OPR<equal_to<size_t>, true, true>,    // eqrr

        // setr: lambda (constexpr -> void)
        [](Registers &regs, size_t A, size_t, size_t C) { regs[C] = regs[A]; },

        // seti: lambda (constexpr -> void)
        [](Registers &regs, size_t A, size_t, size_t C) { regs[C] = A; },

    }}; // end Opcodes
};      // end Device struct

// A way to solve this (in an entirely constant amount of memory) is to utilize
// 16-bit integers and bitwise arithmetic to reduce the "possible" set down to
// its proper equivalences.
void solve() {
    array<uint16_t, 16> OpcodeMap{0}; // opcode map to memo equivalences
    size_t equi3 = 0;                 // part 1 memo
    Device::Registers Before, After;  // the 2 register set states
    Device::Instruction instr;        // the instruction to execute

    // Parse the input and solve for part 1.
    // Before: [3, 2, 1, 1] <- register state (before op)  [0, 1, 2, 3]
    // 9 2 1 2              <- the instr/opcode executed   [OP, Ain, Bin, Cout]
    // After:  [3, 2, 2, 1] <- register state (after op)
    // ...
    while (fscanf(stdin,
                  "Before: [%zu , %zu, %zu, %zu]"
                  "%zu %zu %zu %zu "
                  "After: [%zu , %zu, %zu, %zu] ",
                  &Before[0], &Before[1], &Before[2], &Before[3], &instr[0],
                  &instr[1], &instr[2], &instr[3], &After[0], &After[1],
                  &After[2], &After[3]) == 12) {

        // Execute each instruction on the register's "before" state
        size_t equiCnt{0}; // keep count of how many opcodes behave equivalently
        for (size_t i = 0; i < Device::Opcodes.size(); ++i) {
            // set register to "before" state
            Device::Registers curState{Before};

            // execute an instruction on the register state
            Device::Opcodes[i](curState, instr[1], instr[2], instr[3]);

            // if the register's "before" state is the same as the register's
            // "after" state, count this opcode/instruction as an equivalence
            if (equal(curState.begin(), curState.end(), After.begin())) {
                OpcodeMap[instr[0]] |= (1 << i); // memo validity of opcode
                equiCnt++;                       // add 1 to equivalent count
            }
        }
        equi3 += (equiCnt >= 3); // part 1
    }

    // Part 2: Work out the unique opcode of each operation
    // Keep iterating until all mappings are reduced to unique equivalences.
    bool notAllUnique;
    do {
        notAllUnique = // an opcode's "equivalence" bit isn't unique (part 1)
            any_of(OpcodeMap.begin(), OpcodeMap.end(), [](const uint16_t &Set) {
                return __builtin_popcount(Set) != 1; // counts 1's
            });

        // Unset a unique opcode's equivalence bit from other opcodes
        for (size_t i = 0; i < OpcodeMap.size(); ++i) {
            // Only 1 bit set in this set, solved.
            if (__builtin_popcount(OpcodeMap[i]) == 1) {
                // Remove it from all the other sets.
                for (size_t j = 0; j < OpcodeMap.size(); ++j) {
                    if (i == j) // skip itself
                        continue;
                    OpcodeMap[j] &= ~(OpcodeMap[i]); // bitmask removal
                }
            }
        }
    } while (notAllUnique);

    // Log2 bitmasks into integers.
    for (size_t i = 0; i < OpcodeMap.size(); ++i) // count leading 0's
        OpcodeMap[i] = 32 - __builtin_clz(OpcodeMap[i]) - 1;

    Device::Registers regs{0};

    // Parse the input for part 2.
    while (fscanf(stdin, "%zu %zu %zu %zu ", &instr[0], &instr[1], &instr[2],
                  &instr[3]) == 4)

        // Execute the program for the current input.
        Device::Opcodes[OpcodeMap[instr[0]]](regs, instr[1], instr[2],
                                             instr[3]); // part 2 in regs[0]

    // Part 1: How many input samples behave like three or more opcodes?
    cout << "[Part 01] = " << equi3 << endl; // 493

    // Part 2: Work out the number of each opcode and execute the test program.
    // What value is contained in register 0 after executing the test program?
    cout << "[Part 02] = " << regs[0] << endl; // 445
}

// Main: Time the solver.
int main() {
    // Speed up C++ io by unsyncing with C stdio and untie cin/cout.
    ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    // Time the solve() function in milliseconds.
    auto start_time = std::chrono::high_resolution_clock::now();
    solve();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto ms_count = std::chrono::duration_cast<std::chrono::milliseconds>(
                        end_time - start_time)
                        .count();
    std::clog << "[Runtime] = " << ms_count << "ms" << std::endl;
}