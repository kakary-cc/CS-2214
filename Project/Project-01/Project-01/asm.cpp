//
//  CS 2214
//  Project-01
//
//  Jeff Epstein
//  Starter code
//
//  Mingxuan Zhang
//  E20 assembler
//

#include <cstddef>
#include <iostream>
#include <string>
#include <algorithm>
#include <regex>
#include <vector>
#include <map>
#include <fstream>
#include <bitset>

using namespace std;

/*
    print_machine_code(address, num)
    Print a line of machine code in the required format.
    Parameters:
        address = RAM address of the instructions
        num = numeric value of machine instruction
    */
void print_machine_code(unsigned address, uint16_t num) {
    bitset<16> instruction_in_binary(num);
    cout << "ram[" << address << "] = 16'b" << instruction_in_binary << ";" << endl;
}

// convert to 2's complement
uint8_t twos_complement_7b(int num) {
    if (num < 0) {
        num *= -1;
        num = ~num - (0b1 << 7) + 0b1;
    }
    return num;
}

uint16_t twos_complement_13b(int num) {
    if (num < 0) {
        num *= -1;
        num = ~num - (0b111 << 13) + 0b1;
    }
    return num;
}

uint16_t twos_complement_16b(int num) {
    if (num < 0) {
        num *= -1;
        num = ~num + 0b1;
    }
    return num;
}

// check if immediate value is a label
bool is_label(const string& imm) {
    size_t imm_check = 0;
    try {
        stoi(imm, &imm_check);
    }
    catch(invalid_argument) {
        return true;  // label entirely non-numbers
    }
    if (imm_check == imm.length())
        return false;
    return true;
}

// template for three registers instructions (add, sub, or, and, slt, jr)
uint16_t three_reg_inst(const string& regDst, const string& regSrcA, const string& regSrcB) {
    return (stoi(regSrcA) << 10) + (stoi(regSrcB) << 7) + (stoi(regDst) << 4);
}

// template for two registers and one (absoulte) immediate instructions (slti, lw, sw, addi)
uint16_t reg_imm_inst(const string& regDst, const string& regSrc_Addr, const string& imm, const map<string, int>& labels) {
    if (is_label(imm))
        return (stoi(regSrc_Addr) << 10) + (stoi(regDst) << 7) + labels.at(imm);
    else
        return (stoi(regSrc_Addr) << 10) + (stoi(regDst) << 7) + twos_complement_7b(stoi(imm));
}

// template for two registers and one (relative) immediate instruction (jeq)
uint16_t jmp_if_eq(const string& regA, const string& regB, const string& imm, const map<string, int>& labels, int pc) {
    int rel_imm = is_label(imm) ? labels.at(imm) : stoi(imm);
    rel_imm -= (pc + 1);
    return (stoi(regA) << 10) + (stoi(regB) << 7) + twos_complement_7b(rel_imm);
}

// template for no register instructions (j, jal)
uint16_t jmp(const string& imm, const map<string, int>& labels) {
    if (is_label(imm))
        return labels.at(imm);
    else
        return twos_complement_13b(stoi(imm));
}

// template for .fill
uint16_t fill(const string& imm, const map<string, int>& labels) {
    if (is_label(imm))
        return labels.at(imm);
    else
        return twos_complement_16b(stoi(imm));
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
    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0) {
            if (arg == "-h" || arg == "--help")
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
        cerr << "Assemble E20 files into machine code" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing assembly language, typically with .s suffix" << endl << endl;
        cerr << "optional arguments:" << endl;
        cerr << "  -h, --help  show this help message and exit" << endl;
        return 1;
    }

    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file " << filename << endl;
        return 1;
    }

    map<string, int> labels;
    vector< vector<string> > parsed_lines;
    vector<uint16_t> bin_codes;
    
    // comment removal, label detection, code parse
    string line;
    regex parser(R"([\s|,|$|(|)]+)");
    for (int i = 0; getline(f, line);) {
        // comment removal
        size_t pos = line.find("#");
        if (pos != string::npos)
            line = line.substr(0, pos);
        // leading space removal
        while (line.size() > 0 && isspace(line[0]))
            line = line.substr(1, line.size());
        // case unification
        transform(line.begin(), line.end(), line.begin(), ::tolower);
        // label detection
        while (line.find(":") < line.size()) {
            labels.insert(pair<string, int> (line.substr(0, line.find(":")), i));
            line = line.substr(line.find(":") + 1, line.size());
        }
        // space between label and instruction removal
        while (line.size() > 0 && isspace(line[0]))
            line = line.substr(1, line.size());
        // parse
        if(line.size() > 0) {
            sregex_token_iterator iter(line.begin(), line.end(), parser, -1);
            sregex_token_iterator end;
            vector<string> parsed_line(iter, end);
            parsed_lines.push_back(parsed_line);
            i++;
        }
    }
    
    /* our final output is a list of ints values representing
       machine code instructions */
    uint16_t bin_code;
    for (const vector<string>& parsed_line : parsed_lines) {
        bin_code = 0;
        string op = parsed_line[0];
        
//
//        FOR TESTING PURPOSES
//
//        for(const string& i : parsed_line)
//            cout << i << " ";
//        cout << endl;
//
        
        // three registers instructions
        if (op == "add") {
            bin_code = (0b000 << 13) + 0b0000;
            bin_code += three_reg_inst(parsed_line[1], parsed_line[2], parsed_line[3]);
        }
        if (op == "sub") {
            bin_code = (0b000 << 13) + 0b0001;
            bin_code += three_reg_inst(parsed_line[1], parsed_line[2], parsed_line[3]);
        }
        if (op == "or") {
            bin_code = (0b000 << 13) + 0b0010;
            bin_code += three_reg_inst(parsed_line[1], parsed_line[2], parsed_line[3]);
        }
        if (op == "and") {
            bin_code = (0b000 << 13) + 0b0011;
            bin_code += three_reg_inst(parsed_line[1], parsed_line[2], parsed_line[3]);
        }
        if (op == "slt") {
            bin_code = (0b000 << 13) + 0b0100;
            bin_code += three_reg_inst(parsed_line[1], parsed_line[2], parsed_line[3]);
        }
        if (op == "jr") {
            bin_code = (0b000 << 13) + 0b1000;
            bin_code += three_reg_inst("0", parsed_line[1], "0");
        }
        
        // two registers instructions - calculations (slti, addi)
        if (op == "slti") {
            bin_code = (0b111 << 13);
            bin_code += reg_imm_inst(parsed_line[1], parsed_line[2], parsed_line[3], labels);
        }
        if (op == "addi") {
            bin_code = (0b001 << 13);
            bin_code += reg_imm_inst(parsed_line[1], parsed_line[2], parsed_line[3], labels);
        }
        // two registers instructions - memory accesses (lw, sw)
        if (op == "lw") {
            bin_code = (0b100 << 13);
            bin_code += reg_imm_inst(parsed_line[1], parsed_line[3], parsed_line[2], labels);
        }
        if (op == "sw") {
            bin_code = (0b101 << 13);
            bin_code += reg_imm_inst(parsed_line[1], parsed_line[3], parsed_line[2], labels);
        }
        // two registers instructions - jeq
        if (op == "jeq") {
            bin_code = (0b110 << 13);
            bin_code += jmp_if_eq(parsed_line[1], parsed_line[2], parsed_line[3], labels, (int)bin_codes.size());
        }
        
        // no register instructions
        if (op == "j") {
            bin_code = (0b010 << 13);
            bin_code += jmp(parsed_line[1], labels);
        }
        if (op == "jal") {
            bin_code = (0b011 << 13);
            bin_code += jmp(parsed_line[1], labels);
        }
        
        // pseudo instructions
        if (op == "movi") {  // addi
            bin_code = (0b001 << 13);
            bin_code += reg_imm_inst(parsed_line[1], "0", parsed_line[2], labels);
        }
        if (op == "nop") {  // add
            bin_code = 0;
        }
        if (op == "halt") {  // j
            bin_code = (0b010 << 13);
            bin_code += jmp(to_string(bin_codes.size()), labels);
        }
        
        // assembler directives
        if (op == ".fill") {
            bin_code = fill(parsed_line[1], labels);
        }
        
        bin_codes.push_back(bin_code);
    }

    /* print out each instruction in the required format */
    unsigned addr = 0;
    for (uint16_t bin_code : bin_codes) {
        print_machine_code(addr, bin_code);
        addr++;
    }
    
//
//    FOR TESTING PURPOSES
//
//    cout << "----- LABELS -----\n";
//    for(auto i : labels)
//        cout << i.first << " = " << i.second << endl;
//    cout << "----- INSTRUCTIONS -----\n";
//    for(auto i : parsed_lines) {
//        for(auto j : i)
//            cout << "\"" << j << "\" ";
//        cout << endl;
//    }
//
    
    return 0;
}
