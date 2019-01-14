// Advent of Code 2018
// Day 21: Chronal Conversion
// https://adventofcode.com/2018/day/21

#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <iterator>
#include <set>
#include <unordered_set>
#include <vector>
using namespace std;

// Define the device to simulate as per the given specs.
struct Device {
    typedef array<uintmax_t, 6> Registers;    //  [0, 1, 2, 3, 4, 5]
    typedef array<uintmax_t, 4> Instruction;  //  [OP, Ain, Bin, Cout]
    typedef void (*Op)(Registers&, size_t, size_t, size_t);  // fn pointer

    Registers regs;  // set of 6 registers in the device

    // opcode template: register vs. immediate "specialization" via bool flags
    template <typename F, bool RegA = true, bool RegB = true>
    static void OPR(Registers& regs, size_t A, size_t B, size_t C) {
        F function;  // the operation to execute
        regs[C] = function(RegA ? regs[A] : A, RegB ? regs[B] : B);  // execute
    }

    // the opcode definitions
    constexpr static array<Op, 16> Opcodes = {{
        OPR<plus<size_t>, true, true>,         // addr
        OPR<plus<size_t>, true, false>,        // addi
        OPR<multiplies<size_t>, true, true>,   // mulr
        OPR<multiplies<size_t>, true, false>,  // muli
        OPR<bit_and<size_t>, true, true>,      // banr
        OPR<bit_and<size_t>, true, false>,     // bani
        OPR<bit_or<size_t>, true, true>,       // borr
        OPR<bit_or<size_t>, true, false>,      // bori
        OPR<greater<size_t>, false, true>,     // gtir
        OPR<greater<size_t>, true, false>,     // gtri
        OPR<greater<size_t>, true, true>,      // gtrr
        OPR<equal_to<size_t>, false, true>,    // eqir
        OPR<equal_to<size_t>, true, false>,    // eqri
        OPR<equal_to<size_t>, true, true>,     // eqrr

        // setr: lambda (constexpr -> void)
        [](Registers& regs, size_t A, size_t, size_t C) { regs[C] = regs[A]; },

        // seti: lambda (constexpr -> void)
        [](Registers& regs, size_t A, size_t, size_t C) { regs[C] = A; },

    }};  // end Opcodes
};       // end Device struct

// Convert instruction string names to integer opcodes
size_t opNameToOpcode(const string& opName) {
    vector<string> opNames({"addr", "addi", "mulr", "muli", "banr", "bani",
                            "borr", "bori", "gtir", "gtri", "gtrr", "eqir",
                            "eqri", "eqrr", "setr", "seti"});
    auto opIndex(find(opNames.begin(), opNames.end(), opName));
    return distance(opNames.begin(), opIndex);
}

// Output a register set's full state
ostream& operator<<(ostream& os, const Device::Registers& regs) {
    for (uintmax_t const& state : regs)
        os << state << " ";
    return os;
}

// Parse the input program instruction set
// #ip 2        <- instruction pointer value register #
// seti 123 0 3 <- the instr/opcode executed [OP, Ain, Bin, Cout]
// ...
istream& operator>>(istream& is, vector<Device::Instruction>& v) {
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

// Short-circuit disassembly (Johnny5 lol)
// eqrr tests for the halting condition, so read it's registers for the value
void solve() {
    vector<Device::Instruction> instrs;  // input instruction set to execute
    Device::Registers regs{0};           // the register set [0,1,2,3,4,5]
    Device::Instruction instr;           // the current instruction
    size_t ipReg;                        // instruction pointer register #

    // Parse the input program instruction set
    // #ip 2        <- instruction pointer value register #
    // seti 123 0 3 <- the instr/opcode executed [OP, Ain, Bin, Cout]
    // ...
    string skip;
    cin >> skip >> ipReg >> instrs;
    size_t programSize = instrs.size();

    // Halt value cycle detection variables
    // uintmax_t maxSteps = 1847;
    // regs[0] = 4797782;
    // set<uintmax_t> haltValues;
    unordered_set<uintmax_t> haltValues;
    uintmax_t minHaltValue{0};  // part 1
    uintmax_t maxHaltValue{0};  // part 2

    // Find equality testing instruction for reg[0]
    // e.g. [Line #28] eqrr 3 0 4
    size_t eqrrOpcode{opNameToOpcode("eqrr")};
    size_t eqrrLine{0};        // program line# of eqrr instruction
    uintmax_t eqrrTestReg{0};  // the register# eqrr tests against
    for (auto& ops : instrs) {
        if (ops[0] == eqrrOpcode) {
            eqrrTestReg = (ops[1] != 0) ? ops[1] : ops[2];
            break;
        }
        eqrrLine++;
    }

    // Where is eqrr in the program? What register is tested against reg[0]?
    cerr << "eqrrline=" << eqrrLine << " testreg=" << eqrrTestReg << endl;

    // Run the input program
    // do {
    for (regs = {0}; regs[ipReg] < programSize; ++regs[ipReg]) {
        // state before execution
        // cerr << "ip=" << regs[ipReg] << " [" << regs << "] " << endl;

        // execute the next instuction (indexed by ip register value)
        instr = instrs[regs[ipReg]];
        Device::Opcodes[instr[0]](regs, instr[1], instr[2], instr[3]);

        // state after execution
        // cerr << "ip=" << regs[ipReg] << " [" << regs << "] " << endl;

        // Whenever the ip is pointing to instruction eqrr, we know the program
        // is testing for equality b/w reg[eqrrTestReg] and reg[0] (the halt
        // condition). The first time eqrr is encountered, we get the value for
        // part 1. For part 2, it's the last value before a cycle occurs.
        if (regs[ipReg] == eqrrLine) {
            uintmax_t haltValue = regs[eqrrTestReg];
            if (haltValues.empty())
                minHaltValue = haltValue;  // 4797782
            if (haltValues.count(haltValue))
                break;  // values cycling, done!

            maxHaltValue = haltValue;  // 6086461
            haltValues.insert(haltValue);
        }
    }  // while (++regs[ipReg] < programSize); // increase instruction pointer

    // Debug print a step (last step here)
    cerr << "ip=" << regs[ipReg] << " [ " << regs << "] " << endl;
    cerr << "# of halt values=" << haltValues.size() << endl;  // 10180

    // Part 1:What is the lowest non-negative integer value for register 0 that
    // causes the program to halt after executing the fewest instructions?
    // (Executing the same instruction multiple times counts as multiple
    // instructions executed.)
    cout << "[Part 01] = " << minHaltValue << endl;  // 4797782

    // Part 2: What is the lowest non-negative integer value for register 0 that
    // causes the program to halt after executing the most instructions? (The
    // program must actually halt; running forever does not count as halting.)
    cout << "[Part 02] = " << maxHaltValue << endl;  // 6086461
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