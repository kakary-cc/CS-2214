#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <limits>
#include <regex>
#include <iomanip>

using namespace std;

size_t const static NUM_REGS = 8;
size_t const static MEM_SIZE = 1 << 13;
size_t const static REG_SIZE = 1 << 16;

//
//    Loads an E20 machine code file into the list
//    provided by mem. We assume that mem is
//    large enough to hold the values in the machine
//    code file.
//
//    @param f Open file to read from
//    @param mem Array represetnting memory into which to read program
//
void load_machine_code(ifstream & f, uint16_t mem[]) {
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
        expectedaddr++;
        mem[addr] = instr;
    }
}

//
//    Retrieves 3 register arguments from the instruction.
//
void retrieve(uint16_t inst, uint8_t * regA, uint8_t * regB, uint8_t * regDst) {
    * regA = (inst >> 10) & 0b111;
    * regB = (inst >> 7) & 0b111;
    * regDst = (inst >> 4) & 0b111;
}

//
//    Retrieves 2 register arguments from the instruction.
//
void retrieve(uint16_t inst, uint8_t * regA, uint8_t * regB, uint16_t * imm) {
    * regA = ((inst >> 10) & 0b111);
    * regB = ((inst >> 7) & 0b111);
    * imm = inst & 0b1111111;
}

//
//    Retrieves no register argument from the instruction.
//
void retrieve(uint16_t inst, uint16_t * imm) {
    * imm = inst & 0b1111111111111;
}

//
//    Converts a 7-bit two's complement number to a C++ signed number.
//
int8_t to_signed(uint16_t num) {
    int8_t res = num;
    if ((num >> 6) == 0b1) {
        res = ~num + 0b1 - (0b1 << 7);
        res *= -1;
    }
    return res;
}

/*
    Prints out the correctly-formatted configuration of a cache.

    @param cache_name The name of the cache. "L1" or "L2"

    @param size The total size of the cache, measured in memory cells.
        Excludes metadata

    @param assoc The associativity of the cache. One of [1,2,4,8,16]

    @param blocksize The blocksize of the cache. One of [1,2,4,8,16,32,64])

    @param num_rows The number of rows in the given cache.
 */
void print_cache_config(const string & cache_name, int size, int assoc, int blocksize, int num_rows) {
    cout << "Cache " << cache_name << " has size " << size <<
        ", associativity " << assoc << ", blocksize " << blocksize <<
        ", rows " << num_rows << endl;
}

/*
    Prints out a correctly-formatted log entry.

    @param cache_name The name of the cache where the event
        occurred. "L1" or "L2"

    @param status The kind of cache event. "SW", "HIT", or
        "MISS"

    @param pc The program counter of the memory
        access instruction

    @param addr The memory address being accessed.

    @param row The cache row or set number where the data
        is stored.
 */
void print_log_entry(const string & cache_name,
    const string & status, int pc, int addr, int row) {
    cout << left << setw(8) << cache_name + " " + status << right <<
        " pc:" << setw(5) << pc <<
        "\taddr:" << setw(5) << addr <<
        "\trow:" << setw(4) << row << endl;
}

/*
    Simulates one level of cache.
 */
class Cache {
    private:
        struct Block {
            bool valid;
            int tag;
            //        vector<int> data;
            Block(): valid(false) {};
            Block(int tag): valid(true), tag(tag) {};
        };
    string cache_name;
    int rows, assoc, blocksize;
    /*
        vector: rows
        list: blocks within a row
     */
    vector < list < Block >> cache;

    public:
        Cache(string name, int rows, int assoc, int blocksize): cache_name(name), rows(rows), assoc(assoc), blocksize(blocksize) {
            for (int i = 0; i < rows; i++) {
                cache.push_back(list < Block > ());
                for (int j = 0; j < assoc; j++)
                    cache.back().push_back(Block());
            }
        }
    bool memory_read(int addr, int pc) {
        int block_id = addr / blocksize;
        //        int offset = addr % blocksize;
        int row = block_id % rows;
        int tag = block_id / rows;
        for (auto it = cache[row].begin(); it != cache[row].end(); it++) {
            if (it -> valid && it -> tag == tag) {
                Block found = * it;
                /*
                    HIT
                    Updates LRU.
                 */
                cache[row].erase(it);
                cache[row].push_back(found);
                print_log_entry(cache_name, "HIT", pc, addr, row);
                //                return found.data[offset];
                return true;
            }
        }
        /*
            MISS
            Writes block into cache.
         */
        cache[row].push_back(Block(tag));
        //        cache[row].push_back(Block(tag, /* Actual Data */));
        cache[row].pop_front();
        print_log_entry(cache_name, "MISS", pc, addr, row);
        //        return -1;
        return false;
    }
    void memory_write(int addr, int pc) {
        int block_id = addr / blocksize;
        //        int offset = addr % blocksize;
        int row = block_id % rows;
        int tag = block_id / rows;
        for (auto it = cache[row].begin(); it != cache[row].end(); it++) {
            if (it -> valid && it -> tag == tag) {
                /*
                    EXIST
                    Updates data and LRU.
                 */
                cache[row].erase(it);
                cache[row].push_back(Block(tag));
                //                cache[row].push_back(Block(tag, /* Actual Data */));
                print_log_entry(cache_name, "SW", pc, addr, row);
                return;
            }
        }
        /*
            NON-EXIST
            Writes block into cache.
         */
        //        cache[row].push_back(Block(tag, /* Actual Data */));
        cache[row].push_back(Block(tag));
        cache[row].pop_front();
        print_log_entry(cache_name, "SW", pc, addr, row);
    }
};

/*
    Main function
    Takes command-line args as documented below
*/
int main(int argc, char * argv[]) {
    /*
        Parse the command-line arguments
    */
    char * filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    string cache_config;
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0) {
            if (arg == "-h" || arg == "--help")
                do_help = true;
            else if (arg == "--cache") {
                i++;
                if (i >= argc)
                    arg_error = true;
                else
                    cache_config = argv[i];
            } else
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
        cerr << "usage " << argv[0] << " [-h] [--cache CACHE] filename" << endl << endl;
        cerr << "Simulate E20 cache" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl << endl;
        cerr << "optional arguments:" << endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        cerr << "  --cache CACHE  Cache configuration: size,associativity,blocksize (for one" << endl;
        cerr << "                 cache) or" << endl;
        cerr << "                 size,associativity,blocksize,size,associativity,blocksize" << endl;
        cerr << "                 (for two caches)" << endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file " << filename << endl;
        return 1;
    }

    /* parse cache config */
    if (cache_config.size() > 0) {
        vector < int > parts;
        size_t pos;
        size_t lastpos = 0;
        while ((pos = cache_config.find(",", lastpos)) != string::npos) {
            parts.push_back(stoi(cache_config.substr(lastpos, pos)));
            lastpos = pos + 1;
        }
        parts.push_back(stoi(cache_config.substr(lastpos)));

        bool two_levels;
        int L1size, L1assoc, L1blocksize, L1rows;
        int L2size = 0, L2assoc = 0, L2blocksize = 0, L2rows = 0;
        if (parts.size() == 3) {
            two_levels = false;
            L1size = parts[0];
            L1assoc = parts[1];
            L1blocksize = parts[2];
            L1rows = L1size / (L1assoc * L1blocksize);
            print_cache_config("L1", L1size, L1assoc, L1blocksize, L1rows);
        } else if (parts.size() == 6) {
            two_levels = true;
            L1size = parts[0];
            L1assoc = parts[1];
            L1blocksize = parts[2];
            L1rows = L1size / (L1assoc * L1blocksize);
            L2size = parts[3];
            L2assoc = parts[4];
            L2blocksize = parts[5];
            L2rows = L2size / (L2assoc * L2blocksize);
            print_cache_config("L1", L1size, L1assoc, L1blocksize, L1rows);
            print_cache_config("L2", L2size, L2assoc, L2blocksize, L2rows);
        } else {
            cerr << "Invalid cache config" << endl;
            return 1;
        }
        /*
            Executes E20 program and simulate cache(s)
         */
        Cache L1("L1", L1rows, L1assoc, L1blocksize);
        Cache L2("L2", L2rows, L2assoc, L2blocksize);
        uint16_t pc = 0;
        uint16_t regs[NUM_REGS] = {0};
        uint16_t mem[MEM_SIZE] = {0};
        load_machine_code(f, mem);
        uint8_t regA, regB, regDst;
        uint16_t imm;
        int8_t signed_imm;
        for (bool halt = false; !halt; pc++) {
            // prevents pc overflow
            if (pc == 8192)
                pc = 0;
            uint16_t inst = mem[pc];
            regs[0] = 0;
            switch (inst >> 13) { // opCode
                // -- THREE REGISTER ARGUMENTS INSTRUCTIONS --
            case 0b000:
                retrieve(inst, & regA, & regB, & regDst);
                switch (inst & 0b1111) {
                case 0b0000: // add
                    regs[regDst] = regs[regA] + regs[regB];
                    break;
                case 0b0001: // sub
                    regs[regDst] = regs[regA] - regs[regB];
                    break;
                case 0b0010: // or
                    regs[regDst] = regs[regA] | regs[regB];
                    break;
                case 0b0011: // and
                    regs[regDst] = regs[regA] & regs[regB];
                    break;
                case 0b0100: // slt
                    regs[regDst] = regs[regA] < regs[regB];
                    break;
                case 0b1000: // jr
                    if (regs[regA] == pc)
                        halt = true;
                    pc = regs[regA] - 1;
                    break;
                }
                break;
                // -- TWO REGISTER ARGUMENTS INSTRUCTIONS --
            case 0b111: // slti
                // imm unsigned
                retrieve(inst, & regA, & regB, & imm);
                regs[regB] = regs[regA] < imm;
                break;
            case 0b100: // lw
                // imm signed
                retrieve(inst, & regA, & regB, & imm);
                signed_imm = to_signed(imm);
                if (L1.memory_read(regs[regA] + signed_imm, pc) == false && two_levels)
                    L2.memory_read(regs[regA] + signed_imm, pc);
                regs[regB] = mem[regs[regA] + signed_imm];
                break;
            case 0b101: // sw
                // imm signed
                retrieve(inst, & regA, & regB, & imm);
                signed_imm = to_signed(imm);
                L1.memory_write(regs[regA] + signed_imm, pc);
                if (two_levels)
                    L2.memory_write(regs[regA] + signed_imm, pc);
                mem[regs[regA] + signed_imm] = regs[regB];
                break;
            case 0b110: // jeq
                // imm signed
                retrieve(inst, & regA, & regB, & imm);
                if (regs[regA] != regs[regB])
                    break;
                signed_imm = to_signed(imm);
                if (signed_imm == -1)
                    halt = true;
                pc += signed_imm;
                break;
            case 0b001: // addi
                // imm signed
                retrieve(inst, & regA, & regB, & imm);
                signed_imm = to_signed(imm);
                regs[regB] = regs[regA] + signed_imm;
                break;
                // -- NO REGISTER ARGUMENT INSTRUCTIONS --
            case 0b010: // j
                retrieve(inst, & imm);
                if (pc == imm)
                    halt = true;
                pc = imm - 1;
                break;
            case 0b011: // jal
                retrieve(inst, & imm);
                if (pc == imm)
                    halt = true;
                regs[7] = pc + 1;
                pc = imm - 1;
                break;
            }
        }

    }
    return 0;
}
