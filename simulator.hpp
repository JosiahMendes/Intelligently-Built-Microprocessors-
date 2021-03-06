#ifndef simulator_hpp
#define simulator_hpp

#include <iostream>
#include <string>
#include <assert.h>
#include <map>
#include <vector>
using namespace std;

class CPU {
private:
    bool stop;
    string reg0; // assuming reg 00
    string reg1; // assuming reg 01
    string reg2; // assuming reg 10
    string reg3; // assuming reg 11
    int pc;
    int m_carry;
    map<int,pair<string,string>> m_instructions;
    map<string,string> memory; // location and data written as string
    vector<string> stack; // stack with 16 bit data stored
    string ir; // current instruction (in hex)
    string ir_explained; // ir in words e.g. ADR 0000
public:
    CPU() {
        stop = false;
        reg0 = "0000000000000000";
        reg1 = "0000000000000000";
        reg2 = "0000000000000000";
        reg3 = "0000000000000000";
        pc = 0;
        m_carry = 0;
        ir = "0000";
        ir_explained = "";
    }

    // read instructions and store them in m_instructions
    void read() {
        string instruction;
        string immediate;
        int order = 0;
        while(1) {
            cin >> instruction;
            if(cin.fail()) {
                break;
            }
            assert(instruction.length() == 3);
            instruction = transform_to_uppercase(instruction);
            if(instruction == "ADR" || 
                instruction == "ADM" ||
                instruction == "SBR" || 
                instruction == "SBM" ||
                instruction == "MLR" ||
                instruction == "BBO" ||
                instruction == "XSL" ||
                instruction == "XSR" ||
                instruction == "ADI" ||
                instruction == "SBI" ||
                instruction == "LDI" ||
                instruction == "LDR" ||
                instruction == "LDA" ||
                instruction == "STA" ||
                instruction == "STI" ||
                instruction == "STK" ||
                instruction == "JMR" ||
                instruction == "JMP" ||
                instruction == "JEQ" ||
                instruction == "JNQ" ||
                instruction == "STP") {
                cin >> immediate;
                if(stoi(immediate,nullptr,2)<0) { // immediate stored as binary
                    cout << "Error: invalid immediate value (not a binary value)" << endl;
                    return;
                }
                m_instructions.insert(pair<int,pair<string,string>>(order,make_pair(instruction,immediate)));
            }
            else {
                cout << "Error: " << instruction << " is not a valid instruction" << endl;
                return;
            }
            order++;
        }
    }

    // reading instructions in hexadecimal
    void read_hex() {
        string instruction;
        string instruction_binary;
        string opcode;
        int opcode_length;
        string immediate = "";
        int order = 0;
        while(1) {
            cin >> instruction;
            if(cin.fail()) {
                break;
            }
            assert(instruction.length() == 4);
            int instruction_in_decimal = stoi(instruction,nullptr,16);
            instruction_binary = decimal_to_binary(instruction_in_decimal);
            assert(instruction_binary.length() == 16);
            if(instruction_binary[0] == '0') { // 0xxxx
                if(instruction_binary[1] == '0') { // 00xxx
                    if(instruction_binary[2] == '0') { // 000xx
                        if(instruction_binary[3] == '0') { // 0000x
                            if(instruction_binary[4] == '0') { // 00000 STP
                                opcode = "STP";
                                opcode_length = 5;
                            } else { // 00001 ADR
                                opcode = "ADR";
                                opcode_length = 5;
                            }
                        } else { // 0001 ADM
                            opcode = "ADM";
                            opcode_length = 4;
                        }
                    } else { // 001xx
                        if(instruction_binary[3] == '0') { // 0010x
                            if(instruction_binary[4] == '0') { // 00100 ADI
                                opcode = "ADI";
                                opcode_length = 5;
                            } else { // 00101 SBR
                                opcode = "SBR";
                                opcode_length = 5;
                            }
                        } else { // 0011 SBM
                            opcode = "SBM";
                            opcode_length = 4;
                        }
                    }
                } else { // 01xxx
                    if(instruction_binary[2] == '0') { // 010xx
                        if(instruction_binary[3] == '0') { // 0100x
                            if(instruction_binary[4] == '0') { // 01000 SBI
                                opcode = "SBI";
                                opcode_length = 5;
                            } else { // 01001 MLR
                                opcode = "MLR";
                                opcode_length = 5;
                            }
                        } else { // 0101x
                            if(instruction_binary[4] == '0') { // 01010 XSL
                                opcode = "XSL";
                                opcode_length = 5;
                            } else { // 01011 XSR
                                opcode = "XSR";
                                opcode_length = 5;
                            }
                        }
                    } else { // 011xx
                        if(instruction_binary[3] == '0') { // 0110x
                            if(instruction_binary[4] == '0') { // 01100 BBO
                                opcode = "BBO";
                                opcode_length = 5;
                            } else { // 01101 STK
                                opcode = "STK";
                                opcode_length = 5;
                            }
                        } else { // 0111x
                            if(instruction_binary[4] == '0') { // 01110 LDR
                                opcode = "LDR";
                                opcode_length = 5;
                            } else { // 01111 STI
                                opcode = "STI";
                                opcode_length = 5;
                            }
                        }
                    }
                }
            } else { // 1xxxx
                if(instruction_binary[1] == '0') { // 10xxx
                    if(instruction_binary[2] == '0') { // 100 LDI
                        opcode = "LDI";
                        opcode_length = 3;
                    } else { // 101 STA
                        opcode = "STA";
                        opcode_length = 3;
                    }
                } else { // 11xxx
                    if(instruction_binary[2] == '0') { // 110 LDA
                        opcode = "LDA";
                        opcode_length = 3;
                    } else { // 111xx
                        if(instruction_binary[3] == '0') { // 1110x
                            if(instruction_binary[4] == '0') { // 11100 JMR
                                opcode = "JMR";
                                opcode_length = 5;
                            } else { // 11101 JMP
                                opcode = "JMP";
                                opcode_length = 5;
                            }
                        } else { // 1111x
                            if(instruction_binary[4] == '0') { // 11110 JEQ
                                opcode = "JEQ";
                                opcode_length = 5;
                            } else { // 11111 JNQ
                                opcode = "JNQ";
                                opcode_length = 5;
                            }
                        }
                    }
                }
            }
            //cout << "instruction: " << instruction << " binary: " << instruction_binary << " opcode: " << opcode << endl;
            for(int i = opcode_length; i < 16; i++) {
                immediate = immediate + instruction_binary[i];
            }
            assert(immediate.length() == 16-opcode_length);
            m_instructions.insert(pair<int,pair<string,string>>(order,make_pair(opcode,immediate)));
            immediate = "";
            order++;
        }
    }

    // execute the corresponding instruction
    void execute() {
        map<int,pair<string,string>>::iterator it = m_instructions.find(pc);
        if(it != m_instructions.end()) { // found
            string current_instruction = it->second.first;
            string current_immediate = it->second.second;
            if(current_instruction == "ADR") {
                ADR(current_immediate);
            } else if(current_instruction == "ADM") {
                ADM(current_immediate);
            } else if(current_instruction == "SBR") {
                SBR(current_immediate);
            } else if(current_instruction == "SBM") {
                SBM(current_immediate);
            } else if(current_instruction == "MLR") {
                MLR(current_immediate);
            } else if(current_instruction == "BBO") {
                BBO(current_immediate);
            } else if(current_instruction == "XSL") {
                XSL(current_immediate);
            } else if(current_instruction == "XSR") {
                XSR(current_immediate);
            } else if(current_instruction == "ADI") {
                ADI(current_immediate);
            } else if(current_instruction == "SBI") {
                SBI(current_immediate);
            } else if(current_instruction == "LDI") {
                LDI(current_immediate);
            } else if(current_instruction == "LDR") {
                LDR(current_immediate);
            } else if(current_instruction == "LDA") {
                LDA(current_immediate);
            } else if(current_instruction == "STA") {
                STA(current_immediate);
            } else if(current_instruction == "STI") {
                STI(current_immediate);
            } else if(current_instruction == "STK") {
                STK(current_immediate);
            } else if(current_instruction == "JMR") {
                JMR(current_immediate);
            } else if(current_instruction == "JMP") {
                JMP(current_immediate);
            } else if(current_instruction == "JEQ") {
                JEQ(current_immediate);
            } else if(current_instruction == "JNQ") {
                JNQ(current_immediate);
            } else if(current_instruction == "STP") {
                STP(current_immediate);
            }
        } else { // didn't find instruction
            cout << "no instruction found for the current pc value: " << pc << endl;
            STP("00000000000");
        }
        show_content();
    }

    // run the instructions stored in m_instructions
    void run() {
        show_content();
        while(1) {
            execute();
            if(stop) {
                return;
            }
        }
    }

    // test function to check if read() works
    void print_instructions() {
        if(m_instructions.empty()) {
            cout << "no instructions" << endl;
        } else {
            for(map<int,pair<string,string>>::iterator it = m_instructions.begin(); it != m_instructions.end(); ++it) {
                cout << "PC: " << it->first << " " << "Instruction: " << it->second.first << " " << "Immediate: " << it->second.second << endl;
            }
        }
    }

    void show_content() {
        cout << "PC: " << pc << " (0x" << binary_to_hex(decimal_to_binary(pc)) << ")" << endl;
        cout << "--REGISTERS--" << endl;
        cout << "R0: " << reg0 << " (0x" << binary_to_hex(reg0) << ")" << endl;
        cout << "R1: " << reg1 << " (0x" << binary_to_hex(reg1) << ")" << endl;
        cout << "R2: " << reg2 << " (0x" << binary_to_hex(reg2) << ")" << endl;
        cout << "R3: " << reg3 << " (0x" << binary_to_hex(reg3) << ")" << endl;
        cout << "Stack: ";
        if(stack.empty()) {
            cout << "empty" << endl;
        } else {
            cout << stack.size() << endl;
            for(int i = stack.size()-1; i >= 0; i--) {
                cout << stack[i] << " (0x" << binary_to_hex(stack[i]) << ")" << endl;
            }
        }
        cout << "Carry: " << m_carry << endl;
        cout << "Instruction: 0x" << ir << " // " << ir_explained << endl;
        cout << "--MEMORY--" << " size: " << memory.size() << endl;
        for(map<string,string>::iterator it = memory.begin(); it != memory.end(); ++it) {
            cout << "Location: " << it->first << " (0x" << binary_to_hex(it->first) << ")" << " " << "Data: " << it->second << " (0x" << binary_to_hex(it->second) << ")" << endl;
        }
        cout << "Uninitialized memory is zeroed" << endl;
        cout << "\n\n";
    }

    // Rn := Rn + Rm + Cin
    void ADR(string immediate) {
        assert(immediate.length() == 11);
        bool write_carry = false; // assuming instr[10] = 0
        if(immediate[10-10] == '1') { // if instr[10] = 1, write carry
            write_carry = true;
        }
        string register_Rx = get_register_value(string(1,immediate[10-5])+string(1,immediate[10-4])); // instr[5..4] gives register Rx
        assert(register_Rx.length() == 16);
        string register_Rn = get_register_value(string(1,immediate[10-3])+string(1,immediate[10-2])); // instr[3..2] gives register Rn
        assert(register_Rn.length() == 16);
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0])); // instr[1..0] gives register Rm
        assert(register_Rm.length() == 16);
        int carry_in = 0; // assuming instr[9..8] = 00
        string which_carry_in = string(1,immediate[10-9]) + string(1,immediate[10-8]);
        if(which_carry_in == "01") { // if instr[9..8] = 01, cin = 1
            carry_in = 1;
        } else if(which_carry_in == "10") { // if instr[9..8] = 10, cin = carry
            carry_in = m_carry;
        } else if(which_carry_in == "11") { // if instr[9..8] = 11, cin = MSB of Rm
            if(register_Rm[15-15] == '0') {
                carry_in = 0;
            } else if(register_Rm[15-15] == '1') {
                carry_in = 1;
            }
        }
        string immediate_offset = string(1,immediate[10-7]) + string(1,immediate[10-6]);
        pair<string,bool> result;
        if(immediate_offset == "00") { // no offset
            result = addition(register_Rn, register_Rm, carry_in);
        } else if(immediate_offset == "01") { // + Rx
            pair<string,bool> temp_result = addition(register_Rn, register_Rm, carry_in);
            result = addition(temp_result.first, register_Rx, 0);
            if(temp_result.second) { // if overflow happens in temp_result
                result.second = true;
            }
        } else if(immediate_offset == "10") { // Rm logical shift left by Rx
            string shifted_Rm = left_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = addition(register_Rn, shifted_Rm, carry_in);
        } else if(immediate_offset == "11") { // Rm logical shift right by Rx
            string shifted_Rm = right_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = addition(register_Rn, shifted_Rm, carry_in);
        }
        assert(result.first.length() == 16);
        if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "00") { // if Rn = reg0
            reg0 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "01") { // if Rn = reg1
            reg1 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "10") { // if Rn = reg2
            reg2 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "11") { // if Rn = reg3
            reg3 = result.first;
        }
        if(write_carry) {
            if(result.second) {
                m_carry = 1;
            } else {
                m_carry = 0;
            }
        }
        ir = binary_to_hex("00001"+immediate);
        ir_explained = "ADR " + immediate;
        pc++;
    }

    // R0 := Rn + Mem[N], carry always written
    void ADM(string immediate) {
        assert(immediate.length() == 12);
        string location = "00000";
        for(int i = 11-10; i <= 11-0; i++) {
            location = location + immediate[i];
        }
        assert(location.length() == 16);
        string Rn;
        if(immediate[11-11] == '0') { // choose R0
            Rn = reg0;
        } else {
            Rn = reg1;
        }
        map<string,string>::iterator it = memory.find(location);
        string data;
        if(it != memory.end()) { // found Mem[N]
            data = it->second;
        } else {
            data = "0000000000000000";
        }
        assert(data.length() == 16);
        pair<string,bool> result = addition(Rn, data, 0); // cin = 0
        assert(result.first.length() == 16);
        if(immediate[11-11] == '0') {
            reg0 = result.first;
        } else {
            reg1 = result.first;
        }
        if(result.second) { // if cout = 1
            m_carry = 1;
        } else {
            m_carry = 0;
        }
        ir = binary_to_hex("0001"+immediate);
        ir_explained = "ADM " + immediate;
        pc++;
    }

    // Rn := Rn - Rm - cin
    void SBR(string immediate) {
        assert(immediate.length() == 11);
        bool write_carry = false; // assuming instr[10] = 0
        if(immediate[10-10] == '1') { // if instr[10] = 1, write carry
            write_carry = true;
        }
        string register_Rx = get_register_value(string(1,immediate[10-5])+string(1,immediate[10-4]));
        assert(register_Rx.length() == 16);
        string register_Rn = get_register_value(string(1,immediate[10-3])+string(1,immediate[10-2]));
        assert(register_Rn.length() == 16);
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0]));
        assert(register_Rm.length() == 16);
        int carry_in = 0; // assuming instr[9..8] = 0
        string which_carry_in = string(1,immediate[10-9]) + string(1,immediate[10-8]);
        if(which_carry_in == "01") { // cin = 1
            carry_in = 1;
        } else if(which_carry_in == "10") { // cin = carry
            carry_in = m_carry;
        } else if(which_carry_in == "11") { // cin = MSB of Rm
            if(register_Rm[15-15] == '0') {
                carry_in = 0;
            } else if(register_Rm[15-15] == '1') {
                carry_in = 1;
            }
        }
        string immediate_offset = string(1,immediate[10-7]) + string(1,immediate[10-6]);
        pair<string,bool> result;
        if(immediate_offset == "00") { // no offset
            result = subtraction(register_Rn, register_Rm, carry_in);
        } else if(immediate_offset == "01") { // + Rx
            pair<string,bool> temp_result = addition(register_Rn, register_Rx, 0);
            result = subtraction(temp_result.first, register_Rm, carry_in);
            if(temp_result.second) {
                result.second = true;
            }
        } else if(immediate_offset == "10") { // Rm logical shift left by Rx
            string shifted_Rm = left_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = subtraction(register_Rn, shifted_Rm, carry_in);
        } else if(immediate_offset == "11") { // Rm logical shift right by Rx
            string shifted_Rm = right_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = subtraction(register_Rn, shifted_Rm, carry_in);
        }
        assert(result.first.length() == 16);
        if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "00") { // if Rn = reg0
            reg0 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "01") { // if Rn = reg1
            reg1 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "10") { // if Rn = reg2
            reg2 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "11") { // if Rn = reg3
            reg3 = result.first;
        }
        if(write_carry) {
            if(result.second) {
                m_carry = 1;
            } else {
                m_carry = 0;
            }
        }
        ir = binary_to_hex("00101"+immediate);
        ir_explained = "SBR " + immediate;
        pc++;
    }

    // R0 := R0 - Mem[N], carry always written
    void SBM(string immediate) {
        assert(immediate.length() == 12);
        string location = "00000";
        for(int i = 11-10; i <= 11-0; i++) {
            location = location + immediate[i];
        }
        assert(location.length() == 16);
        string Rn;
        if(immediate[11-11] == '0') { // choose R0
            Rn = reg0;
        } else {
            Rn = reg1;
        }
        map<string,string>::iterator it = memory.find(location);
        string data;
        if(it != memory.end()) { // found Mem[N]
            data = it->second;
        } else {
            data = "0000000000000000";
        }
        assert(data.length() == 16);
        pair<string,bool> result = subtraction(Rn, data, 0); // cin = 0
        assert(result.first.length() == 16);
        if(immediate[11-11] == '0') {
            reg0 = result.first;
        } else {
            reg1 = result.first;
        }
        if(result.second) { // if cout = 1
            m_carry = 1;
        } else {
            m_carry = 0;
        }
        ir = binary_to_hex("0011"+immediate);
        ir_explained = "SBM " + immediate;
        pc++;
    }

    // Rn := Rn * Rm + cin
    void MLR(string immediate) {
        assert(immediate.length() == 11);
        bool write_carry = false; // assuming instr[10] = 0
        if(immediate[10-10] == '1') { // if instr[10] = 1, write carry
            write_carry = true;
        }
        string register_Rx = get_register_value(string(1,immediate[10-5])+string(1,immediate[10-4])); // instr[5..4] gives register Rx
        assert(register_Rx.length() == 16);
        string register_Rn = get_register_value(string(1,immediate[10-3])+string(1,immediate[10-2])); // instr[3..2] gives register Rn
        assert(register_Rn.length() == 16);
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0])); // instr[1..0] gives register Rm
        assert(register_Rm.length() == 16);
        int carry_in = 0; // assuming instr[9..8] = 00
        string which_carry_in = string(1,immediate[10-9]) + string(1,immediate[10-8]);
        if(which_carry_in == "01") { // if instr[9..8] = 01, cin = 1
            carry_in = 1;
        } else if(which_carry_in == "10") { // if instr[9..8] = 10, cin = carry
            carry_in = m_carry;
        } else if(which_carry_in == "11") { // if instr[9..8] = 11, cin = MSB of Rm
            if(register_Rm[15-15] == '0') {
                carry_in = 0;
            } else if(register_Rm[15-15] == '1') {
                carry_in = 1;
            }
        }
        string immediate_offset = string(1,immediate[10-7]) + string(1,immediate[10-6]);
        pair<string,bool> result;
        if(immediate_offset == "00") { // no offset
            result = multiplication(register_Rn, register_Rm, carry_in);
        } else if(immediate_offset == "01") { // + Rx
            pair<string,bool> temp_result = multiplication(register_Rn, register_Rm, carry_in);
            result = addition(temp_result.first, register_Rx, 0);
            if(temp_result.second) { // if overflow happens in temp_result
                result.second = true;
            }
        } else if(immediate_offset == "10") { // Rm logical shift left by Rx
            string shifted_Rm = left_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = multiplication(register_Rn, shifted_Rm, carry_in);
        } else if(immediate_offset == "11") { // Rm logical shift right by Rx
            string shifted_Rm = right_shift(register_Rm, stoi(register_Rx,nullptr,2)%16);
            result = multiplication(register_Rn, shifted_Rm, carry_in);
        }
        assert(result.first.length() == 16);
        if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "00") { // if Rn = reg0
            reg0 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "01") { // if Rn = reg1
            reg1 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "10") { // if Rn = reg2
            reg2 = result.first;
        } else if(string(1,immediate[10-3])+string(1,immediate[10-2]) == "11") { // if Rn = reg3
            reg3 = result.first;
        }
        if(write_carry) {
            if(result.second) {
                m_carry = 1;
            } else {
                m_carry = 0;
            }
        }
        ir = binary_to_hex("01001"+immediate);
        ir_explained = "MLR " + immediate;
        pc++;
    }

    // Bitwise Boolean Operations, carry NOT written
    void BBO(string immediate) {
        assert(immediate.length() == 11);
        string which_boolean_operation = string(1,immediate[10-10])+string(1,immediate[10-9])+string(1,immediate[10-8]);
        string register_Rn = get_register_value(string(1,immediate[10-3])+string(1,immediate[10-2]));
        assert(register_Rn.length() == 16);
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0]));
        assert(register_Rm.length() == 16);
        string N_value = "";
        for(int i = 10-7; i <= 10-4; i++) {
            N_value = N_value + immediate[i];
        }
        assert(N_value.length() == 4);
        int n = stoi(N_value,nullptr,2);
        string result;
        string zero_register = "0000000000000000";
        if(which_boolean_operation == "000") { // Rn = N + NOT Rm
            string not_Rm = bitwise_not(register_Rm);
            assert(not_Rm.length() == 16);
            pair<string,bool> sum = addition(not_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "001") { // Rn = Rn AND Rm + N
            string Rn_and_Rm = bitwise_and(register_Rn,register_Rm);
            assert(Rn_and_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_and_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "010") { // Rn = Rn OR Rm + N
            string Rn_or_Rm = bitwise_or(register_Rn,register_Rm);
            assert(Rn_or_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_or_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "011") { // Rn = Rn XOR Rm + N
            string Rn_xor_Rm = bitwise_xor(register_Rn,register_Rm);
            assert(Rn_xor_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_xor_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "100") { // Rn = Rn NAND Rm + N
            string Rn_and_Rm = bitwise_and(register_Rn,register_Rm);
            string Rn_nand_Rm = bitwise_not(Rn_and_Rm);
            assert(Rn_nand_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_nand_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "101") { // Rn = Rn NOR Rm + N
            string Rn_or_Rm = bitwise_or(register_Rn,register_Rm);
            string Rn_nor_Rm = bitwise_not(Rn_or_Rm);
            assert(Rn_nor_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_nor_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "110") { // Rn = Rn XNOR Rm + N
            string Rn_xor_Rm = bitwise_xor(register_Rn,register_Rm);
            string Rn_xnor_Rm = bitwise_not(Rn_xor_Rm);
            assert(Rn_xnor_Rm.length() == 16);
            pair<string,bool> sum = addition(Rn_xnor_Rm,zero_register,n);
            result = sum.first;
        } else if(which_boolean_operation == "111") { // Rn = Rm + N
            pair<string,bool> sum = addition(register_Rm,zero_register,n);
            result = sum.first;
        }
        assert(result.length() == 16);
        string which_register = string(1,immediate[10-3])+string(1,immediate[10-2]);
        if(which_register == "00") {
            reg0 = result;
        } else if(which_register == "01") {
            reg1 = result;
        } else if(which_register == "10") {
            reg2 = result;
        } else if(which_register == "11") {
            reg3 = result;
        }
        ir = binary_to_hex("01100"+immediate);
        ir_explained = "BBO " + immediate;
        pc++;
    }

    // Rn := Rn + N, carry always written
    void ADI(string immediate) {
        assert(immediate.length() == 11);
        string register_Rn = get_register_value(string(1,immediate[10-10])+string(1,immediate[10-9]));
        string immediate_value_16 = "0000000";
        for(int i = 10-8; i <= 10-0; i++) {
            immediate_value_16 = immediate_value_16 + immediate[i];
        }
        assert(immediate_value_16.length() == 16);
        pair<string,bool> result = addition(register_Rn,immediate_value_16,0);
        assert(result.first.length() == 16);
        string which_register = string(1,immediate[10-10])+string(1,immediate[10-9]);
        if(which_register == "00") {
            reg0 = result.first;
        } else if(which_register == "01") {
            reg1 = result.first;
        } else if(which_register == "10") {
            reg2 = result.first;
        } else if(which_register == "11") {
            reg3 = result.first;
        }
        if(result.second) {
            m_carry = 1;
        } else {
            m_carry = 0;
        }
        ir = binary_to_hex("00100"+immediate);
        ir_explained = "ADI " + immediate;
        pc++;
    }

    // Rn := Rn - N
    void SBI(string immediate) {
        assert(immediate.length() == 11);
        string register_Rn = get_register_value(string(1,immediate[10-10])+string(1,immediate[10-9]));
        string immediate_value_16 = "0000000";
        for(int i = 10-8; i <= 10-0; i++) {
            immediate_value_16 = immediate_value_16 + immediate[i];
        }
        assert(immediate_value_16.length() == 16);
        pair<string,bool> result = subtraction(register_Rn,immediate_value_16,0);
        assert(result.first.length() == 16);
        string which_register = string(1,immediate[10-10])+string(1,immediate[10-9]);
        if(which_register == "00") {
            reg0 = result.first;
        } else if(which_register == "01") {
            reg1 = result.first;
        } else if(which_register == "10") {
            reg2 = result.first;
        } else if(which_register == "11") {
            reg3 = result.first;
        }
        if(result.second) {
            m_carry = 1;
        } else {
            m_carry = 0;
        }
        ir = binary_to_hex("01000"+immediate);
        ir_explained = "SBI " + immediate;
        pc++;
    }

    // Rn := N
    void LDI(string immediate) { // only 3 bits for instruction, so 13 bits for immediate
        assert(immediate.length() == 13);
        string which_register = string(1,immediate[12-12])+string(1,immediate[12-11]);
        string immediate_value_16 = "00000";
        for(int i = 12-10; i <= 12-0; i++) {
            immediate_value_16 = immediate_value_16 + immediate[i];
        }
        assert(immediate_value_16.length() == 16);
        if(which_register == "00") {
            reg0 = immediate_value_16;
        } else if(which_register == "01") {
            reg1 = immediate_value_16;
        } else if(which_register == "10") {
            reg2 = immediate_value_16;
        } else if(which_register == "11") {
            reg3 = immediate_value_16;
        }
        ir = binary_to_hex("100"+immediate);
        ir_explained = "LDI " + immediate;
        pc++;
    }

    // Rn := Mem[Ra+N] or Mem[Ra+Rb<<s]
    void LDR(string immediate) {
        assert(immediate.length() == 11);
        string which_register = string(1,immediate[10-10])+string(1,immediate[10-9]);
        string register_holding_address = string(1,immediate[10-7])+string(1,immediate[10-6]);
        string r_a_value;
        if(register_holding_address == "00") {
            r_a_value = reg0;
        } else if(register_holding_address == "01") {
            r_a_value = reg1;
        } else if(register_holding_address == "10") {
            r_a_value = reg2;
        } else if(register_holding_address == "11") {
            r_a_value = reg3;
        }
        assert(r_a_value.length() == 16);
        pair<string,bool> sum;
        if(immediate[10-8] == '0') { // register with immediate offset. Rn := mem[Ra+N]
            string immediate_value_16 = "0000000000";
            for(int i = 10-5; i <= 10-0; i++) {
                immediate_value_16 = immediate_value_16 + immediate[i];
            }
            assert(immediate_value_16.length() == 16);
            sum = addition(r_a_value,immediate_value_16,0);
        } else if(immediate[10-8] == '1') { // register with scaled register offset. Rn := mem[Ra+Rb<<s]
            string register_holding_offset = string(1,immediate[10-5])+string(1,immediate[10-4]);
            string r_b_value;
            if(register_holding_offset == "00") {
                r_b_value = reg0;
            } else if(register_holding_offset == "01") {
                r_b_value = reg1;
            } else if(register_holding_offset == "10") {
                r_b_value = reg2;
            } else if(register_holding_offset == "11") {
                r_b_value = reg3;
            }
            assert(r_b_value.length() == 16);
            string shift_value;
            for(int i = 10-3; i <= 10-0; i++) {
                shift_value = shift_value + immediate[i];
            }
            assert(shift_value.length() == 4);
            string shifted_r_b = left_shift(r_b_value,stoi(shift_value,nullptr,2));
            sum = addition(r_a_value,shifted_r_b,0);
        }
        assert(sum.first.length() == 16);
        map<string,string>::iterator it = memory.find(sum.first);
        string loaded_value;
        if(it != memory.end()) { // found
            loaded_value = it->second;
        } else { // not found
            loaded_value = "0000000000000000";
        }
        assert(loaded_value.length() == 16);
        if(which_register == "00") {
            reg0 = loaded_value;
        } else if(which_register == "01") {
            reg1 = loaded_value;
        } else if(which_register == "10") {
            reg2 = loaded_value;
        } else if(which_register == "11") {
            reg3 = loaded_value;
        }
        ir = binary_to_hex("01110"+immediate);
        ir_explained = "LDR " + immediate;
        pc++;
    }

    // Rn := Mem[N]
    void LDA(string immediate) {
        assert(immediate.length() == 13);
        string which_register = string(1,immediate[12-12])+string(1,immediate[12-11]);
        assert(which_register.length() == 2);
        string data;
        string location = "00000";
        for(int i = 12-10; i <= 12-0; i++) {
            location = location + immediate[i];
        }
        assert(location.length() == 16);
        map<string,string>::iterator it = memory.find(location);
        if(it != memory.end()) { // found
            assert(it->second.length() == 16);
            data = it->second;
        } else { // not found
            data = "0000000000000000";
        }
        if(which_register == "00") {
            reg0 = data;
        } else if(which_register == "01") {
            reg1 = data;
        } else if(which_register == "10") {
            reg2 = data;
        } else if(which_register == "11") {
            reg3 = data;
        }
        ir = binary_to_hex("110"+immediate);
        ir_explained = "LDA " + immediate;
        pc++;
    }

    // Mem[N] = Rn
    void STA(string immediate) {
        assert(immediate.length() == 13);
        string which_register = string(1,immediate[12-12])+string(1,immediate[12-11]);
        assert(which_register.length() == 2);
        string store_value;
        if(which_register == "00") {
            store_value = reg0;
        } else if(which_register == "01") {
            store_value = reg1;
        } else if(which_register == "10") {
            store_value = reg2;
        } else if(which_register == "11") {
            store_value = reg3;
        }
        assert(store_value.length() == 16);
        string location = "00000";
        for(int i = 12-10; i <= 12-0; i++) {
            location = location + immediate[i];
        }
        assert(location.length() == 16);
        map<string,string>::iterator it = memory.find(location);
        if(it != memory.end()) { // found
            it->second = store_value;
        } else { // not found
            memory.insert(pair<string,string>(location,store_value));
        }
        ir = binary_to_hex("101"+immediate);
        ir_explained = "STA " + immediate;
        pc++;
    }

    // store indirect
    void STI(string immediate) {
        assert(immediate.length() == 11);
        string register_Rn = get_register_value(string(1,immediate[10-10])+string(1,immediate[10-9]));
        assert(register_Rn.length() == 16);
        if(immediate[10-8] == '0') { // mem[Ra + N] = Rn
            string register_Ra = get_register_value(string(1,immediate[10-7])+string(1,immediate[10-6]));
            assert(register_Ra.length() == 16);
            string N_value = "0000000000";
            for(int i = 10-5; i <= 10-0; i++) {
                N_value = N_value + immediate[i];
            }
            assert(N_value.length() == 16);
            pair<string,bool> sum = addition(register_Ra,N_value,0);
            assert(sum.first.length() == 16);
            map<string,string>::iterator it = memory.find(sum.first);
            if(it != memory.end()) { // found
                it->second = register_Rn;
            } else { // not found
                memory.insert(pair<string,string>(sum.first,register_Rn));
            }
        } else if(immediate[10-8] == '1') { // mem[Ra + Rb << s] = Rn
            string register_Ra = get_register_value(string(1,immediate[10-7])+string(1,immediate[10-6]));
            string register_Rb = get_register_value(string(1,immediate[10-5])+string(1,immediate[10-4]));
            string s_value = "";
            for(int i = 10-3; i <= 10-0; i++) {
                s_value = s_value + immediate[i];
            }
            assert(s_value.length() == 4);
            string shifted_Rb = left_shift(register_Rb,stoi(s_value,nullptr,2));
            assert(shifted_Rb.length() == 16);
            pair<string,bool> sum = addition(register_Ra,shifted_Rb,0);
            assert(sum.first.length() == 16);
            map<string,string>::iterator it = memory.find(sum.first);
            if(it != memory.end()) { // found
                it->second = register_Rn;
            } else { // not found
                memory.insert(pair<string,string>(sum.first,register_Rn));
            }
        }
        ir = binary_to_hex("01111"+immediate);
        ir_explained = "STI " + immediate;
        pc++;
    }

    // adding and removing from the stack
    void STK(string immediate) {
        assert(immediate.length() == 11);
        if(immediate[10-10] == '0') { // PSH
            PSH(immediate);
        } else if(immediate[10-10] == '1') { // POP
            POP(immediate);
        }
        ir = binary_to_hex("01101"+immediate);
        ir_explained = "STK " + immediate;
        pc++;
    }

    // adding to the stack
    void PSH(string immediate) {
        assert(immediate.length() == 11);
        string which_register = string(1,immediate[10-9])+string(1,immediate[10-8])+string(1,immediate[10-7]);
        string pushed_value;
        string n_value = "0000000000";
        if(which_register == "000") {
            pushed_value = reg0;
        } else if(which_register == "001") {
            pushed_value = reg1;
        } else if(which_register == "010") {
            pushed_value = reg2;
        } else if(which_register == "011") {
            pushed_value = reg3;
        } else if(which_register == "100") {
            pushed_value = decimal_to_binary(pc);
        } else {
            cout << "wrong register value" << endl;
        }
        assert(pushed_value.length() == 16);
        pair<string,bool> result;
        for(int i = 10-5; i <= 10-0; i++) {
            n_value = n_value + immediate[i];
        }
        assert(n_value.length() == 16);
        if(immediate[10-6] == '0') {
            result = addition(pushed_value,n_value,0);
        } else {
            result = subtraction(pushed_value,n_value,0);
        }
        assert(result.first.length() == 16);
        stack.push_back(result.first);
    }

    // removing from the stack
    void POP(string immediate) {
        assert(immediate.length() == 11);
        string which_register = string(1,immediate[10-9])+string(1,immediate[10-8])+string(1,immediate[10-7]);
        string popped_value;
        if(stack.empty()) {
            popped_value = "0000000000000000";
        } else {
            popped_value = stack[stack.size()-1];
        }
        assert(popped_value.length() == 16);
        if(which_register == "000") {
            reg0 = popped_value;
        } else if(which_register == "001") {
            reg1 = popped_value;
        } else if(which_register == "010") {
            reg2 = popped_value;
        } else if(which_register == "011") {
            reg3 = popped_value;
        } else if(which_register == "100") {
            pc = stoi(popped_value,nullptr,2);
        }
        if(!stack.empty()) {
            stack.resize(stack.size()-1);
        }
    }

    // Rn := Rm XSL N
    void XSL(string immediate) {
        assert(immediate.length() == 11);
        bool write_carry = false; // assuming instr[10] = 0
        if(immediate[10-10] == '1') { // if instr[10] = 1, write carry
            write_carry = true;
        }
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0]));
        assert(register_Rm.length() == 16);
        string which_carry_in = string(1,immediate[10-9])+string(1,immediate[10-8]);
        char carry_in = '0'; // assuming instr[9..8] = 00
        if(which_carry_in == "01") {
            carry_in = '1';
        } else if(which_carry_in == "10") {
            if(m_carry == 0) {
                carry_in = '0';
            } else if(m_carry == 1) {
                carry_in = '1';
            }
        } else if(which_carry_in == "11") {
            if(register_Rm[0] == '0') {
                carry_in = '0';
            } else if(register_Rm[0] == '1') {
                carry_in = '1';
            }
        }
        string immediate_value;
        for(int i = 10-7; i <= 10-4; i++) {
            immediate_value = immediate_value + immediate[i];
        }
        assert(immediate_value.length() == 4);
        string result = register_Rm;
        int n = stoi(immediate_value,nullptr,2);
        for(int i = 0; i <= 15-n; i++) {
            result[i] = register_Rm[i+n];
        }
        for(int i = 16-n; i < 16; i++) {
            result[i] = carry_in;
        }
        assert(result.length() == 16);
        string Rn = string(1,immediate[10-3])+string(1,immediate[10-2]);
        if(Rn == "00") {
            reg0 = result;
        } else if(Rn == "01") {
            reg1 = result;
        } else if(Rn == "10") {
            reg2 = result;
        } else if(Rn == "11") {
            reg3 = result;
        }
        if(write_carry) {
            if(n > 0 && register_Rm[0] == '1') {
                m_carry = 1;
            } else {
                m_carry = 0;
            }
        }
        ir = binary_to_hex("01010"+immediate);
        ir_explained = "XSL " + immediate;
        pc++;
    }

    // Rn := Rm XSR N
    void XSR(string immediate) {
        assert(immediate.length() == 11);
        bool write_carry = false; // assuming instr[10] = 0
        if(immediate[10-10] == '1') { // if instr[10] = 1, write carry
            write_carry = true;
        }
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0]));
        assert(register_Rm.length() == 16);
        string which_carry_in = string(1,immediate[10-9])+string(1,immediate[10-8]);
        char carry_in = '0'; // assuming instr[9..8] = 00
        if(which_carry_in == "01") {
            carry_in = '1';
        } else if(which_carry_in == "10") {
            if(m_carry == 0) {
                carry_in = '0';
            } else if(m_carry == 1) {
                carry_in = '1';
            }
        } else if(which_carry_in == "11") {
            if(register_Rm[0] == '0') {
                carry_in = '0';
            } else if(register_Rm[0] == '1') {
                carry_in = '1';
            }
        }
        string immediate_value;
        for(int i = 10-7; i <= 10-4; i++) {
            immediate_value = immediate_value + immediate[i];
        }
        assert(immediate_value.length() == 4);
        string result = register_Rm;
        int n = stoi(immediate_value,nullptr,2);
        for(int i = n; i <= 15; i++) {
            result[i] = register_Rm[i-n];
        }
        for(int i = 0; i < n; i++) {
            result[i] = carry_in;
        }
        assert(result.length() == 16);
        string Rn = string(1,immediate[10-3])+string(1,immediate[10-2]);
        if(Rn == "00") {
            reg0 = result;
        } else if(Rn == "01") {
            reg1 = result;
        } else if(Rn == "10") {
            reg2 = result;
        } else if(Rn == "11") {
            reg3 = result;
        }
        if(write_carry) {
            if(n > 0 && register_Rm[15] == '1') {
                m_carry = 1;
            } else {
                m_carry = 0;
            }
        }
        ir = binary_to_hex("01011"+immediate);
        ir_explained = "XSR " + immediate;
        pc++;
    }

    // PC = Rm with conditions
    void JMR(string immediate) {
        assert(immediate.length() == 11);
        string condition = string(1,immediate[10-10])+string(1,immediate[10-9])+string(1,immediate[10-8]);
        bool jump = false;
        string register_Rn = get_register_value(string(1,immediate[10-3])+string(1,immediate[10-2]));
        assert(register_Rn.length() == 16);
        int Rn = stoi(register_Rn,nullptr,2);
        string x_value = "";
        for(int i = 10-7; i <= 10-4; i++) {
            x_value = x_value + immediate[i];
        }
        assert(x_value.length() == 4);
        int x = stoi(x_value,nullptr,2);
        string register_Rx = get_register_value(string(1,immediate[10-5])+string(1,immediate[10-4]));
        assert(register_Rx.length() == 16);
        int Rx = stoi(register_Rx,nullptr,2);
        if(condition == "000") {
            if(Rn == 0) { // if Rn = 0
                jump = true;
            }
        } else if(condition == "001") {
            if(Rn != 0) {
                jump = true;
            }
        } else if(condition == "010") {
            if(Rn == Rx) {
                jump = true;
            }
        } else if(condition == "011") {
            if(Rn != Rx) {
                jump = true;
            }
        } else if(condition == "100") {
            if(Rn < Rx) {
                jump = true;
            }
        } else if(condition == "101") {
            if(Rn <= Rx) {
                jump = true;
            }
        } else if(condition == "110") {
            if(register_Rn[15-x] == '1') {
                jump = true;
            }
        } else if(condition == "111") {
            if(m_carry = 1) {
                jump = true;
            }
        }
        string register_Rm = get_register_value(string(1,immediate[10-1])+string(1,immediate[10-0]));
        assert(register_Rm.length() == 16);
        if(jump) {
            pc = stoi(register_Rm,nullptr,2);
        } else {
            pc++;
        }
        ir = binary_to_hex("11100"+immediate);
        ir_explained = "JMR " + immediate;
    }

    // PC = N
    void JMP(string immediate) {
        assert(immediate.length() == 11);
        pc = stoi(immediate,nullptr,2);
        ir = binary_to_hex("11101"+immediate);
        ir_explained = "JMP " + immediate;
    }

    // PC = N if R0 = 0
    void JEQ(string immediate) {
        assert(immediate.length() == 11);
        if(reg0 == "0000000000000000") {
            pc = stoi(immediate,nullptr,2);
        } else {
            pc++;
        }
        ir = binary_to_hex("11110"+immediate);
        ir_explained = "JEQ " + immediate;
    }

    // PC = N if R0 != 0
    void JNQ(string immediate) {
        assert(immediate.length() == 11);
        if(reg0 != "0000000000000000") {
            pc = stoi(immediate,nullptr,2);
        } else {
            pc++;
        }
        ir = binary_to_hex("11111"+immediate);
        ir_explained = "JNQ " + immediate;
    }

    // stops the program from executing further instructions
    void STP(string immediate) {
        stop = true;
        cout << "Stopping on 'STP' instruction at " << pc << endl;
        ir = binary_to_hex("00000"+immediate);
        ir_explained = "STP";
    }

    // logical shift left by n, used for immediate offset
    string left_shift(string immediate, int n) {
        assert(immediate.length() == 16);
        string result = immediate;
        for(int i = 0; i < immediate.length()-n; i++) {
            result[i] = immediate[i+n];
        }
        for(int i = immediate.length()-n; i < immediate.length(); i++) {
            result[i] = '0';
        }
        assert(result.length() == 16);
        return result;
    }

    // logical shift right by n, used for immediate offset
    string right_shift(string immediate, int n) {
        assert(immediate.length() == 16);
        string result = immediate;
        for(int i = n; i < immediate.length(); i++) {
            result[i] = immediate[i-n];
        }
        for(int i = 0; i < n; i++) {
            result[i] = '0';
        }
        assert(result.length() == 16);
        return result;
    }

    // return value at the chosen register
    string get_register_value(string immediate) {
        assert(immediate.length() == 2);
        if(immediate == "00") {
            return reg0;
        } else if(immediate == "01") {
            return reg1;
        } else if(immediate == "10") {
            return reg2;
        } else if(immediate == "11") {
            return reg3;
        }
    }

    // performs Rn + Rm + cin then returns sum and overflow flag
    pair<string,bool> addition(string Rn, string Rm, int cin) {
        bool overflow = false;
        assert(Rn.length() == 16);
        assert(Rm.length() == 16);
        int Rn_value = stoi(Rn, nullptr, 2);
        int Rm_value = stoi(Rm, nullptr, 2);
        int sum = Rn_value + Rm_value + cin;
        string sum_binary = decimal_to_binary(sum);
        if(stoi(sum_binary, nullptr, 2) < sum) { // overflow happened
            overflow = true;
        }
        return pair<string,bool>(sum_binary,overflow);
    }

    // performs Rn - Rm - cin then returns result and cout
    pair<string,bool> subtraction(string Rn, string Rm, int cin) {
        bool overflow = false;
        assert(Rn.length() == 16);
        assert(Rm.length() == 16);
        int Rn_value = stoi(Rn,nullptr,2);
        int Rm_value = stoi(Rm,nullptr,2);
        int result = Rn_value - Rm_value - cin;
        string result_binary = decimal_to_binary(result);
        if(result >= 0 && stoi(result_binary,nullptr,2) == result) {
            // overflow = false;
        } else {
            overflow = true; // might be wrong
        }
        return pair<string,bool>(result_binary,overflow);
    }

    pair<string,bool> multiplication(string Rn, string Rm, int cin) {
        bool overflow = false;
        assert(Rn.length() == 16);
        assert(Rm.length() == 16);
        long Rn_value = stol(Rn, nullptr, 2);
        long Rm_value = stol(Rm, nullptr, 2);
        long result = Rn_value * Rm_value + cin;
        string result_binary = decimal_to_binary(result);
        if(stoi(result_binary, nullptr, 2) < result) { // overflow happened
            overflow = true;
        }
        return pair<string,bool>(result_binary,overflow);
    }

    string bitwise_not(string s) {
        assert(s.length() == 16);
        string result = "";
        for(int i = 0; i < 16; i++) {
            if(s[i] == '0') {
                result = result + '1';
            } else if(s[i] == '1') {
                result = result + '0';
            }
        }
        assert(result.length() == 16);
        return result;
    }

    string bitwise_and(string s1, string s2) {
        assert(s1.length() == 16);
        assert(s2.length() == 16);
        string result = "";
        for(int i = 0; i < 16; i++) {
            if(s1[i] == '0' && s2[i] == '0') {
                result = result + '0';
            } else if(s1[i] == '0' && s2[i] == '1') {
                result = result + '0';
            } else if(s1[i] == '1' && s2[i] == '0') {
                result = result + '0';
            } else if(s1[i] == '1' && s2[i] == '1') {
                result = result + '1';
            }
        }
        assert(result.length() == 16);
        return result;
    }

    string bitwise_or(string s1, string s2) {
        assert(s1.length() == 16);
        assert(s2.length() == 16);
        string result = "";
        for(int i = 0; i < 16; i++) {
            if(s1[i] == '0' && s2[i] == '0') {
                result = result + '0';
            } else if(s1[i] == '0' && s2[i] == '1') {
                result = result + '1';
            } else if(s1[i] == '1' && s2[i] == '0') {
                result = result + '1';
            } else if(s1[i] == '1' && s2[i] == '1') {
                result = result + '1';
            }
        }
        assert(result.length() == 16);
        return result;
    }

    string bitwise_xor(string s1, string s2) {
        assert(s1.length() == 16);
        assert(s2.length() == 16);
        string result = "";
        for(int i = 0; i < 16; i++) {
            if(s1[i] == '0' && s2[i] == '0') {
                result = result + '0';
            } else if(s1[i] == '0' && s2[i] == '1') {
                result = result + '1';
            } else if(s1[i] == '1' && s2[i] == '0') {
                result = result + '1';
            } else if(s1[i] == '1' && s2[i] == '1') {
                result = result + '0';
            }
        }
        assert(result.length() == 16);
        return result;
    }

    void decimal_to_binary_test(int n) {
        cout << decimal_to_binary(n) << endl;
    }

    string decimal_to_binary(long decimal_value) {
        bool positive_value = true;
        if(decimal_value < 0) {
            positive_value = false;
        }
        string binary_value = "";
        long n = abs(decimal_value);
        while(n != 0) {
            if(n % 2 == 0) {
                binary_value = "0" + binary_value;
            } else {
                binary_value = "1" + binary_value;
            }
            n = n / 2;
        }
        if(positive_value) {
            if(binary_value.length() < 16) { // fill the rest with zeroes
                string temp = binary_value;
                for(int i = 0; i < 16-binary_value.length(); i++) {
                    temp = "0" + temp;
                }
                binary_value = temp;
            } else if(binary_value.length() > 16) {// discard excess bits
                string temp = "";
                for(int i = binary_value.length()-1; i >= binary_value.length()-16; i--) {
                    temp = binary_value[i] + temp;
                }
                binary_value = temp;
            }
        } else { // negative value
            string temp = "";
            for(int i = 0; i < binary_value.length(); i++) { // invert all bits
                if(binary_value[i] == '0') {
                    temp = temp + '1';
                } else if(binary_value[i] == '1') {
                    temp = temp + '0';
                }
            }
            temp = "1" + temp;
            long int_temp = stol(temp,nullptr,2) + 1;
            string twos_complement_binary = "";
            while(int_temp != 0) {
                if(int_temp % 2 == 0) {
                    twos_complement_binary = "0" + twos_complement_binary;
                } else {
                    twos_complement_binary = "1" + twos_complement_binary;
                }
                int_temp = int_temp / 2;
            }
            if(twos_complement_binary.length() < 16) { // fill the rest with ones
                string t = twos_complement_binary;
                for(int i = 0; i < 16-twos_complement_binary.length(); i++) {
                    t = "1" + t;
                }
                twos_complement_binary = t;
            } else if(twos_complement_binary.length() > 16) { // discard the excess bits
                string t = "";
                for(int i = twos_complement_binary.length()-1; i >= twos_complement_binary.length()-16; i--) {
                    t = twos_complement_binary[i] + t;
                }
                twos_complement_binary = t;
            }
            if(pc == 1152) {
                cout << twos_complement_binary << endl;
            }
            assert(twos_complement_binary.length() == 16);
            binary_value = twos_complement_binary;
        }
        assert(binary_value.length() == 16);
        return binary_value;
    }

    // turn 16-bit binary to hexadecimal
    string binary_to_hex(string binary_number) {
        assert(binary_number.length() == 16);
        string result = "";
        string four_bit_number;
        for(int i = 0; i <= 12; i = i + 4) {
            four_bit_number = "";
            for(int j = 0; j < 4; j++) {
                four_bit_number = four_bit_number + binary_number[i+j];
            }
            assert(four_bit_number.length() == 4);
            int n = stoi(four_bit_number,nullptr,2);
            if(n < 10) {
                result = result + to_string(n);
            } else {
                switch(n) {
                    case 10:
                        result = result + "A";
                        break;
                    case 11:
                        result = result + "B";
                        break;
                    case 12:
                        result = result + "C";
                        break;
                    case 13:
                        result = result + "D";
                        break;
                    case 14:
                        result = result + "E";
                        break;
                    case 15:
                        result = result + "F";
                        break;
                };
            }
        }
        assert(result.length() == 4);
        return result;
    }

    // turn a string to uppercase
    string transform_to_uppercase(string instr) {
        string transformed = "";
        for(int i = 0; i < instr.length(); i++) {
            transformed += toupper(instr[i]);
        }
        return transformed;
    }
};


#endif