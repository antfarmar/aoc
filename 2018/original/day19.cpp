// Advent of Code 2018
// Day 19: Go With The Flow
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
using namespace std;

// Define the device to simulate as per the given specs.
struct Device {
    typedef array<uintmax_t, 6> Registers;   //  [0, 1, 2, 3, 4, 5]
    typedef array<uintmax_t, 4> Instruction; //  [OP, Ain, Bin, Cout]
    typedef void (*Op)(Registers &, size_t, size_t, size_t); // fn pointer

    Registers regs; // set of 6 regs in the device

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

// Convert instruction string names to integer opcodes
size_t opNameToOpcode(const string &opName) {
    vector<string> opNames({"addr", "addi", "mulr", "muli", "banr", "bani",
                            "borr", "bori", "gtir", "gtri", "gtrr", "eqir",
                            "eqri", "eqrr", "setr", "seti"});

    auto opIndex(find(opNames.begin(), opNames.end(), opName));
    return distance(opNames.begin(), opIndex);
}

// Output a register set's full state
ostream &operator<<(ostream &os, const Device::Registers &regs) {
    for (auto &state : regs)
        os << state << " ";
    return os;
}

// Parse the input program instruction set
istream &operator>>(istream &is, vector<Device::Instruction> &v) {
    while (is.good()) {
        Device::Instruction instr;
        string opName;
        cin >> opName;
        instr[0] = opNameToOpcode(opName);
        for (auto i = 1; i < 4; i++) {
            uintmax_t n;
            cin >> n;
            instr[i] = n;
        }
        v.push_back(instr);
    }
    return is;
}

// Disassemble the program: sum of divisors of register 3
// (O(n^2) process: too long too run for part 2
void solve() {
    vector<Device::Instruction> instrs; // input instruction set to execute
    Device::Registers regs{0};          // the register set
    Device::Instruction instr;          // the current instruction
    size_t ipReg;                       // instruction pointer register #

    // #ip 0        <- instruction pointer value register #
    // seti 5 0 1   <- the instr/opcode executed [OP, Ain, Bin, Cout]
    // ...
    string skip;
    cin >> skip >> ipReg >> instrs;
    size_t programSize = instrs.size();
    uintmax_t reg0at0{0}; // part 1 memo
    uintmax_t reg0at1{0}; // part 2 memo

    // Run the input program
    // do {
    for (size_t part = 1; part <= 2; part++) {
        regs = {0}; // reset registers
        for (regs[0] = part - 1; regs[ipReg] < programSize; ++regs[ipReg]) {
            // state before execution
            // cerr << "ip=" << regs[ipReg] << " [" << regs << "] " << endl;

            // execute the next instuction (indexed by ip register value)
            instr = instrs[regs[ipReg]];
            Device::Opcodes[instr[0]](regs, instr[1], instr[2], instr[3]);

            // part 2: sum of divisors of 10551282 (register 3 @ ip=33)
            if (part == 2 && regs[ipReg] > 32)
                break;

            // state after execution
            // cerr << "ip=" << regs[ipReg] << " [" << regs << "] " << endl;
        } // while (++regs[ipReg] < instrs.size());

        // part 1 memo: value in register 0
        if (part == 1)
            reg0at0 = regs[0];
    }

    // sum of divisors of value in register 3
    reg0at1 = [](auto num) {
        size_t sum{0};
        for (size_t n = 1; n <= sqrt(num); n++) {
            if (num % n == 0)
                sum += (n == (num / n)) ? n : (n + num / n);
        }
        return sum;
    }(regs[2]);

    // Debug print a step (last step here)
    cerr << "ip=" << regs[ipReg] << " [ " << regs << "] " << endl;

    // Part 1: What value is left in register 0 when the bg process halts?
    cout << "[Part 01] = " << reg0at0 << endl; // 2223 (sum of divisors of 882)

    // Part 2: A new background process immediately spins up in its place. It
    // appears identical, but on closer inspection, you notice that this time,
    // register 0 started with the value 1. What value is left in register 0
    // when this new background process halts? (sum of divisors of 10551282)
    cout << "[Part 02] = " << reg0at1 << endl; // 24117312
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