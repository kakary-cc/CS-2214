//
//  CS 2214
//  Project-02
//
//  Jeff Epstein
//  Starter code
//
//  Mingxuan Zhang
//  E20 simulator
//

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1<<13;
size_t const static REG_SIZE = 1<<16;

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
void load_machine_code(ifstream &f, uint16_t mem[]) {
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;
    string line;
    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
        expectedaddr ++;
        mem[addr] = instr;
    }
}

/*
    Prints the current state of the simulator, including
    the current program counter, the current register values,
    and the first memquantity elements of memory.

    @param pc The final value of the program counter
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memquantity How many words of memory to dump
*/
void print_state(uint16_t pc, uint16_t regs[], uint16_t memory[], size_t memquantity) {
    cout << setfill(' ');
    cout << "Final state:" << endl;
    cout << "\tpc=" << setw(5) << pc << endl;

    for (size_t reg=0; reg < NUM_REGS; reg++)
        cout << "\t$" << reg << "=" << setw(5) << regs[reg] << endl;

    cout << setfill('0');
    bool cr = false;
    for (size_t count=0; count<memquantity; count++) {
        cout << hex << setw(4) << memory[count] << " ";
        cr = true;
        if (count % 8 == 7) {
            cout << endl;
            cr = false;
        }
    }
    if (cr)
        cout << endl;
}

/*
    retrieve function
    for instructions with 3 register arguments
*/
void retrieve(uint16_t inst, uint8_t* regA, uint8_t* regB, uint8_t* regDst) {
    *regA = (inst >> 10) & 0b111;
    *regB = (inst >> 7) & 0b111;
    *regDst = (inst >> 4) & 0b111;
}

/*
    retrieve function
    for instructions with 2 register arguments
*/
void retrieve(uint16_t inst, uint8_t* regA, uint8_t* regB, uint16_t* imm) {
    *regA = ((inst >> 10) & 0b111);
    *regB = ((inst >> 7) & 0b111);
    *imm = inst & 0b1111111;
}

/*
    retrieve function
    for instructions with no register argument
*/
void retrieve(uint16_t inst, uint16_t* imm) {
    *imm = inst & 0b1111111111111;
}

/*
    convert a 7-bit two's complement number to a C++ signed number
*/
int8_t to_signed(uint16_t num) {
    int8_t res = num;
    if ((num >> 6) == 0b1) {
        res = ~num + 0b1 - (0b1 << 7);
        res *= -1;
    }
    return res;
}

/*
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char *argv[]) {
    /*
        Parse the command-line arguments
    */
    char *filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-",0)==0) {
            if (arg== "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] filename" << endl << endl;
        cerr << "Simulate E20 machine" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<< filename << endl;
        return 1;
    }
    // TODO: your code here. Load f and parse using load_machine_code
    uint16_t pc = 0;
    uint16_t regs[NUM_REGS] = {0};
    uint16_t mem[MEM_SIZE] = {0};
    size_t memquantity = 128; // MEM_SIZE
    load_machine_code(f, mem);
    // TODO: your code here. Do simulation.
    uint8_t regA, regB, regDst;
    uint16_t imm;
    int8_t signed_imm;
    for (bool halt = false; !halt; pc++) {
        // ignore the most significant 3 bits of pc
        uint16_t inst = mem[pc & 8191];
        regs[0] = 0;
        switch(inst >> 13) {  // opCode
            // -- THREE REGISTER ARGUMENTS INSTRUCTIONS --
            case 0b000:
                retrieve(inst, &regA, &regB, &regDst);
                switch (inst & 0b1111) {
                    case 0b0000:  // add
                        regs[regDst] = regs[regA] + regs[regB];
                        break;
                    case 0b0001:  // sub
                        regs[regDst] = regs[regA] - regs[regB];
                        break;
                    case 0b0010:  // or
                        regs[regDst] = regs[regA] | regs[regB];
                        break;
                    case 0b0011:  // and
                        regs[regDst] = regs[regA] & regs[regB];
                        break;
                    case 0b0100:  // slt
                        regs[regDst] = regs[regA] < regs[regB];
                        break;
                    case 0b1000:  // jr
                        if (regs[regA] == pc)
                            halt = true;
                        pc = regs[regA] - 1;
                        break;
                }
                break;
            // -- TWO REGISTER ARGUMENTS INSTRUCTIONS --
            case 0b111:  // slti
                // imm unsigned
                retrieve(inst, &regA, &regB, &imm);
                regs[regB] = regs[regA] < imm;
                break;
            case 0b100:  // lw
                // imm signed
                retrieve(inst, &regA, &regB, &imm);
                signed_imm = to_signed(imm);
                regs[regB] = mem[regs[regA] + signed_imm];
                break;
            case 0b101:  // sw
                // imm signed
                retrieve(inst, &regA, &regB, &imm);
                signed_imm = to_signed(imm);
                mem[regs[regA] + signed_imm] = regs[regB];
                break;
            case 0b110:  // jeq
                // imm signed
                retrieve(inst, &regA, &regB, &imm);
                if (regs[regA] != regs[regB])
                    break;
                signed_imm = to_signed(imm);
                if (signed_imm == -1)
                    halt = true;
                pc += signed_imm;
                break;
            case 0b001:  // addi
                // imm signed
                retrieve(inst, &regA, &regB, &imm);
                signed_imm = to_signed(imm);
                regs[regB] = regs[regA] + signed_imm;
                break;
            // -- NO REGISTER ARGUMENT INSTRUCTIONS --
            case 0b010:  // j
                retrieve(inst, &imm);
                if (pc == imm)
                    halt = true;
                pc = imm - 1;
                break;
            case 0b011:  // jal
                retrieve(inst, &imm);
                if (pc == imm)
                    halt = true;
                regs[7] = pc + 1;
                pc = imm - 1;
                break;
        }
    }
    // TODO: your code here. print the final state of the simulator before ending, using print_state
    print_state(pc, regs, mem, memquantity);
    return 0;
}
//ra0Eequ6ucie6Jei0koh6phishohm9
