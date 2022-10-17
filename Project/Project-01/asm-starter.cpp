/*
CS-UY 2214
Jeff Epstein
Starter code for E20 assembler
asm.cpp
*/

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <bitset>

using namespace std;


/**
    print_line(address, num)
    Print a line of machine code in the required format.
    Parameters:
        address = RAM address of the instructions
        num = numeric value of machine instruction 
    */
void print_machine_code(unsigned address, unsigned num) {
    bitset<16> instruction_in_binary(num);
    cout << "ram[" << address << "] = 16'b" << instruction_in_binary <<";"<<endl;
}

/**
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
        cerr << "Assemble E20 files into machine code" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing assembly language, typically with .s suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        return 1;
    }

    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<<filename<<endl;
        return 1;
    }

    /* our final output is a list of ints values representing
       machine code instructions */
    vector<unsigned> instructions;
    
    string line;
    while (getline(f, line)) {
        size_t pos = line.find("#");
        if (pos != string::npos)
            line = line.substr(0, pos);
        instructions.push_back(line.size());   // TODO change this. generate the machine code
    }

    /* print out each instruction in the required format */
    unsigned address = 0;
    for (unsigned instruction : instructions) {
        print_machine_code(address, instruction); 
        address ++;
    }
 
    return 0;
}

//ra0Eequ6ucie6Jei0koh6phishohm9