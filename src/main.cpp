#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

void interpret(const std::string& line, std::unordered_map<std::string, int>& variables, std::unordered_map<std::string, bool>& constants, bool& mathMode) {

    if(line.find("#clude <mathz>" == 0)) {
        mathMode = true;
    }else if (mathMode && (line.find("+") != std::string::npos || line.find("*") != std::string::npos)) {
        std::istringstream iss(line);
        int result = 0;
        char op;
        int num;
        
        iss >> result; // Leer el primer número
        while (iss >> op >> num) {
            if (op == '+') {
                result += num;
            } else if (op == '*') {
                result *= num;
            } else if (op == '-') {
                result -= num;
            } else if (op == '/') {
                result /= num;
            } else {
                std::cerr << "Error: Operador no soportado " << op << std::endl;
                return;
            }
        }
        std::cout << "Resultado: " << result << std::endl;
    }

    if (line.find("let") == 0) {
        std::istringstream iss(line.substr(3)); // Eliminar "let"
        std::string varName;
        int value;
        
        if (iss >> varName >> value) {
            variables[varName] = value;
        }
    } else if (line.find("const") == 0) {
        std::istringstream iss(line.substr(5)); // Eliminar "const"
        std::string varName;
        int value;
        
        if (iss >> varName >> value) {
            if (constants.find(varName) == constants.end()) {
                variables[varName] = value;
                constants[varName] = true; // Marcar como constante
            } else {
                std::cerr << "Error: Cannot redefine constant " << varName << std::endl;
            }
        }
    } else if (line.find("printf") == 0) {
        std::istringstream iss(line.substr(6)); // Eliminar "printf"
        std::string content;
        
        if (std::getline(iss, content, ')')) {
            // Eliminar el paréntesis de apertura
            if (content.front() == '(') content.erase(0, 1);
            
            std::string output;
            std::istringstream contentStream(content);
            std::string token;
            
            while (std::getline(contentStream, token, '+')) {
                // Eliminar espacios en blanco
                token.erase(remove_if(token.begin(), token.end(), isspace), token.end());
                
                if (!token.empty()) {
                    if (token.front() == '"' && token.back() == '"') {
                        // Es un string literal
                        token.erase(0, 1); // Eliminar comilla inicial
                        token.pop_back();  // Eliminar comilla final
                        output += token;
                    } else if (variables.find(token) != variables.end()) {
                        // Es una variable
                        output += std::to_string(variables[token]);
                    } else {
                        std::cerr << "Error: Variable " << token << " is not defined." << std::endl;
                        return;
                    }
                }
            }
            
            std::cout << output << std::endl;
        }
    }
    
    else if(line.find("testPrint()") == 0 || line.find("testPrint") == 0) {
        std::cout << "This is a test";
    }
     else if (line.find("print") == 0) {
        std::istringstream iss(line.substr(5)); // Eliminar "print"
        std::string varName, condition;
        int value;
        
        if (iss >> varName >> condition >> value) {
            if (condition == "==" && variables[varName] == value) {
                std::cout << varName << " = " << variables[varName] << std::endl;
                //std::cout << variables[varName] << std::endl;
            }
            // Aquí podrías agregar más operadores como !=, <, >, etc.
        } else {
            // Si no hay condición, simplemente imprime el valor de la variable
            std::cout << "" << variables[varName] << std::endl;
        }
   } else if (line.find("if") == 0) {
        std::istringstream iss(line.substr(2)); // Eliminar "if"
        std::string condition, thenPart;
        if (std::getline(iss, condition, '{')) {
            // Eliminar espacios en blanco al inicio y al final de la condición
            condition.erase(0, condition.find_first_not_of(" \t"));
            condition.erase(condition.find_last_not_of(" \t") + 1);

            std::getline(iss, thenPart, '}'); // Asegurarse de leer hasta el cierre de la llave
            std::istringstream condStream(condition);
            std::string varName, op;
            int value;
            condStream >> varName >> op >> value;

            bool conditionMet = false;
            if (variables.find(varName) != variables.end()) {
                if (op == "==") {
                    conditionMet = (variables[varName] == value);
                } else if (op == "!=") {
                    conditionMet = (variables[varName] != value);
                } else if (op == "<") {
                    conditionMet = (variables[varName] < value);
                } else if (op == ">") {
                    conditionMet = (variables[varName] > value);
                } else if (op == "<=") {
                    conditionMet = (variables[varName] <= value);
                } else if (op == ">=") {
                    conditionMet = (variables[varName] >= value);
                } else {
                    std::cerr << "Error: Operador no soportado " << op << std::endl;
                }
            } else {
                std::cerr << "Error: Variable " << varName << " no definida." << std::endl;
            }

            if (conditionMet) {
                interpret(thenPart, variables, constants, mathMode); // Añade el argumento faltante
            }
        }
    }
}

void executeFile(const std::string& filePath) {
     std::ifstream file(filePath);
    std::string line;
    std::unordered_map<std::string, int> variables;
    std::unordered_map<std::string, bool> constants;
    bool mathMode = false; // Variable para controlar el modo matemático

    if (file.is_open()) {
        while (getline(file, line)) {
            interpret(line, variables, constants, mathMode);
        }
        file.close();
    } else {
        std::cerr << "Could not open file: " << filePath << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: rnode path/to/file.rdev" << std::endl;
        return 1;
    }

    if(std::string(argv[1]) == "--version") {
        std::cout << "RLunar\n\nVERSION:1.0\nBeta: True\n" << std::endl;
        return 0;
    }
    else if(std::string(argv[1]) == "--about") {
        std::cout << "RLunar\n\nAuthor: NopAngel (in GitHub)\nYear: 2024\n" << std::endl;
        return 0;
    }
    
    executeFile(argv[1]);
    return 0;
}
