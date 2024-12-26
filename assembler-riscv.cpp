#include <bits/stdc++.h>
using namespace std;

// Define the Instruction class
class Instruction
{
public:
    string opcode, func3, func7;

    Instruction(string op, string f3, string f7)
    {
        opcode = op;
        func3 = f3;
        func7 = f7;
    }

    virtual string toBinary() const = 0; // Pure virtual function
};

// Register name to number
int regToNum(const string &reg)
{
    return stoi(reg.substr(1));
}

// Immediate value to number (handles sign extension)
int immToNum(const string &imm)
{
    return stoi(imm);
}

// Number to binary string (handles both positive and negative numbers)
string toBinaryString(int num, int len)
{
    string binary = "";
    bool isNegative = (num < 0);

    num = abs(num);
    while (num > 0)
    {
        binary = ((num % 2 == 0) ? "0" : "1") + binary;
        num /= 2;
    }

    while (binary.size() < len)
        binary = "0" + binary;

    if (isNegative)
    {
        // Two's complement for negative numbers
        for (int i = binary.size() - 1; i >= 0; i--)
        {
            if (binary[i] == '1')
            {
                for (int j = i - 1; j >= 0; j--)
                    binary[j] = (binary[j] == '1') ? '0' : '1';
                break;
            }
        }
    }

    return binary;
}

// R-type instruction
class RType : public Instruction
{
public:
    string rd, rs1, rs2;

    RType(string op, string f3, string f7, string rd, string rs1, string rs2)
        : Instruction(op, f3, f7)
    {
        this->rd = toBinaryString(regToNum(rd), 5);
        this->rs1 = toBinaryString(regToNum(rs1), 5);
        this->rs2 = toBinaryString(regToNum(rs2), 5);
    }

    string toBinary() const override
    {
        return func7 + rs2 + rs1 + func3 + rd + opcode;
    }
};

// I-type instruction
class IType : public Instruction
{
public:
    string rd, rs1, imm;

    IType(string op, string f3, string rd, string rs1, string imm)
        : Instruction(op, f3, "")
    {
        this->rd = toBinaryString(regToNum(rd), 5);
        this->rs1 = toBinaryString(regToNum(rs1), 5);
        this->imm = toBinaryString(immToNum(imm), 12);
    }

    string toBinary() const override
    {
        return imm + rs1 + func3 + rd + opcode;
    }
};

// S-type instruction (splitting the immediate into two parts, no reordering)
class SType : public Instruction
{
public:
    string rs1, rs2, imm1, imm2;

    SType(string op, string f3, string rs1, string rs2, string imm)
        : Instruction(op, f3, "")
    {
        this->rs1 = toBinaryString(regToNum(rs1), 5);
        this->rs2 = toBinaryString(regToNum(rs2), 5);
        string immediate = toBinaryString(immToNum(imm), 12);
        this->imm1 = immediate.substr(0, 7);  // imm[11:5]
        this->imm2 = immediate.substr(7, 5);  // imm[4:0]
    }

    string toBinary() const override
    {
        return imm1 + rs2 + rs1 + func3 + imm2 + opcode;
    }
};

// B-type instruction (splitting the immediate into two parts, no reordering)
class BType : public Instruction
{
public:
    string rs1, rs2, imm1, imm2;

    BType(string op, string f3, string rs1, string rs2, string imm)
        : Instruction(op, f3, "")
    {
        this->rs1 = toBinaryString(regToNum(rs1), 5);
        this->rs2 = toBinaryString(regToNum(rs2), 5);
        string immediate = toBinaryString(immToNum(imm), 12);
        this->imm1 = immediate.substr(0, 7);  // imm[11:5]
        this->imm2 = immediate.substr(7, 5);  // imm[4:0]
    }

    string toBinary() const override
    {
        return imm1 + rs2 + rs1 + func3 + imm2 + opcode;
    }
};

// J-type instruction (no rearrangement, only splitting)
class JType : public Instruction
{
public:
    string rd, imm;

    JType(string op, string rd, string imm)
        : Instruction(op, "", "")
    {
        this->rd = toBinaryString(regToNum(rd), 5);
        this->imm = toBinaryString(immToNum(imm), 20);
    }

    string toBinary() const override
    {
        return imm + rd + opcode;
    }
};

// U-type instruction
class UType : public Instruction
{
public:
    string rd, imm;

    UType(string op, string rd, string imm)
        : Instruction(op, "", "")
    {
        this->rd = toBinaryString(regToNum(rd), 5);
        this->imm = toBinaryString(immToNum(imm), 20);
    }

    string toBinary() const override
    {
        return imm + rd + opcode;
    }
};

// Define a struct to hold instruction details
struct InstructionDetails
{
    string opcode;
    string func3;
    string func7;
    string type;
};

// Define a map to hold instruction details
unordered_map<string, InstructionDetails> instructionMap = 
{
    // R-type
    {"ADD", {"0110011", "000", "0000000", "R"}},
    {"SUB", {"0110011", "000", "0100000", "R"}},
    {"XOR", {"0110011", "100", "0000000", "R"}},
    {"OR", {"0110011", "110", "0000000", "R"}},
    {"AND", {"0110011", "111", "0000000", "R"}},
    {"SLL", {"0110011", "001", "0000000", "R"}},
    {"SRL", {"0110011", "101", "0000000", "R"}},
    {"SRA", {"0110011", "101", "0100000", "R"}},
    {"SLT", {"0110011", "010", "0000000", "R"}},
    {"SLTU", {"0110011", "011", "0000000", "R"}},

    // I-type
    {"ADDI", {"0010011", "000", "", "I"}},
    {"XORI", {"0010011", "100", "", "I"}},
    {"ORI", {"0010011", "110", "", "I"}},
    {"ANDI", {"0010011", "111", "", "I"}},
    {"SLLI", {"0010011", "001", "0000000", "I"}},
    {"SRLI", {"0010011", "101", "0000000", "I"}},
    {"SRAI", {"0010011", "101", "0100000", "I"}},
    {"SLTI", {"0010011", "010", "", "I"}},
    {"SLTIU", {"0010011", "011", "", "I"}},
    {"LB", {"0000011", "000", "", "I"}},
    {"LH", {"0000011", "001", "", "I"}},
    {"LW", {"0000011", "010", "", "I"}},
    {"LBU", {"0000011", "100", "", "I"}},
    {"LHU", {"0000011", "101", "", "I"}},
    {"JALR", {"1100111", "000", "", "I"}},
    {"ECALL", {"1110011", "000", "", "I"}},
    {"EBREAK", {"1110011", "000", "", "I"}},

    // S-type
    {"SB", {"0100011", "000", "", "S"}},
    {"SH", {"0100011", "001", "", "S"}},
    {"SW", {"0100011", "010", "", "S"}},

    // B-type
    {"BEQ", {"1100011", "000", "", "B"}},
    {"BNE", {"1100011", "001", "", "B"}},
    {"BLT", {"1100011", "100", "", "B"}},
    {"BGE", {"1100011", "101", "", "B"}},
    {"BLTU", {"1100011", "110", "", "B"}},
    {"BGEU", {"1100011", "111", "", "B"}},

    // U-type
    {"LUI", {"0110111", "", "", "U"}},
    {"AUIPC", {"0010111", "", "", "U"}},

    // J-type
    {"JAL", {"1101111", "", "", "J"}}
};

// Function to parse and create instruction object
Instruction *createInstruction(const string &instName, const vector<string> &operands)
{
    auto it = instructionMap.find(instName);
    if (it != instructionMap.end())
    {
        const InstructionDetails &details = it->second;

        if (details.type == "R")
        {
            if (operands.size() == 3)
                return new RType(details.opcode, details.func3, details.func7, operands[0], operands[1], operands[2]);
        }
        else if (details.type == "I")
        {
            if (operands.size() == 3)
                return new IType(details.opcode, details.func3, operands[0], operands[1], operands[2]);
        }
        else if (details.type == "S")
        {
            if (operands.size() == 3)
                return new SType(details.opcode, details.func3, operands[0], operands[1], operands[2]);
        }
        else if (details.type == "B")
        {
            if (operands.size() == 3)
                return new BType(details.opcode, details.func3, operands[0], operands[1], operands[2]);
        }
        else if (details.type == "J")
        {
            if (operands.size() == 2)
                return new JType(details.opcode, operands[0], operands[1]);
        }
        else if (details.type == "U")
        {
            if (operands.size() == 2)
                return new UType(details.opcode, operands[0], operands[1]);
        }
    }
    return nullptr;
}

// Function to split a string based on a delimiter (space or comma)
vector<string> splitString(const string &str)
{
    vector<string> tokens;
    string token;
    for (char ch : str)
    {
        if (ch == ' ' || ch == ',')
        {
            if (!token.empty())
            {
                tokens.push_back(token); // Add the token to the list
                token.clear();           // Clear the token for the next one
            }
        }
        else
            token += ch; // Append character to the current token
    }
    if (!token.empty())
        tokens.push_back(token); // Add the last token

    return tokens;
}

// Function to process and generate machine code
void processInstruction(const string &instructionStr, vector<string> &binaryInst)
{
    vector<string> tokens = splitString(instructionStr);

    // Check if we have at least an instruction name
    if (tokens.empty())
    {
        cout << "Invalid instruction format" << endl;
        return;
    }

    string instName = tokens[0];                               // First token is the instruction name
    vector<string> operands(tokens.begin() + 1, tokens.end()); // Rest are operands

    // Create instruction object
    Instruction *inst = createInstruction(instName, operands);

    if (inst)
    {
        // Print the 32-bit binary representation of the instruction
        binaryInst.push_back(inst->toBinary());
        delete inst;
    }
    else
        cout << "Invalid instruction or operands" << endl;
}

// Main function
int main()
{
    // Assembly code with commas
    int n;
    cout << "Number of Instructions: ";
    cin >> n;
    cin.ignore();

    cout << "Enter Instructions: " << endl;
    vector<string> instructions(n);
    for(int i = 0; i < n; i++)
        getline(cin, instructions[i]);

    // Process each instruction
    vector<string> binaryInst;
    for (const auto &inst : instructions)
        processInstruction(inst, binaryInst);

    for(int i = 0; i < n; i++)
        cout << binaryInst[i] << endl;

    return 0;
}
