#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <regex>
#include <stack>
#include <cmath>
#include <sstream>

/*
Compiler was written with no knowledge of actual compilers...
How I would make a compiler. NO VIBE CODING, NO SEARCHING GOOGLE (or any search engine) FOR COMPLETE PROJECT

Programming language name = Scratchpiler ;-)

Takes in file named "main.txt" which should be in the same folder as this
*/

double evaluateExpression(const std::string& expr) {
    // Tokenize the input
    std::vector<std::string> tokens;
    std::string num;
    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue;
        if (isdigit(c) || c == '.') {
            num += c;
        } else {
            if (!num.empty()) {
                tokens.push_back(num);
                num.clear();
            }
            if (std::string("+-*/%^()").find(c) != std::string::npos)
                tokens.emplace_back(1, c);
        }
    }
    if (!num.empty()) tokens.push_back(num);

    // Convert infix to postfix
    std::vector<std::string> output;
    std::stack<char> ops;

    auto precedence = [](char op) {
        if (op == '^') return 3;
        if (op == '*' || op == '/' || op == '%') return 2;
        if (op == '+' || op == '-') return 1;
        return 0;
    };
    auto isRightAssociative = [](char op) {
        return op == '^';
    };
    for (const auto& token : tokens) {
        if (isdigit(token[0]) || (token.size() > 1 && isdigit(token[1]))) {
            output.push_back(token);
        } else if (token == "(") {
            ops.push('(');
        } else if (token == ")") {
            while (!ops.empty() && ops.top() != '(') {
                output.emplace_back(1, ops.top());
                ops.pop();
            }
            if (!ops.empty()) ops.pop();  // remove '('
        } else {
            char op = token[0];
            while (!ops.empty() && (
                (precedence(ops.top()) > precedence(op)) ||
                (precedence(ops.top()) == precedence(op) && !isRightAssociative(op))
            ) && ops.top() != '(') {
                output.emplace_back(1, ops.top());
                ops.pop();
            }
            ops.push(op);
        }
    }
    while (!ops.empty()) {
        output.emplace_back(1, ops.top());
        ops.pop();
    }

    // Evaluate postfix
    std::stack<double> eval;
    for (const auto& token : output) {
        if (isdigit(token[0]) || (token.size() > 1 && (isdigit(token[1]) || token[1] == '.'))) {
            eval.push(std::stod(token));
        } else {
            double b = eval.top(); eval.pop();
            double a = eval.top(); eval.pop();
            switch (token[0]) {
                case '+': eval.push(a + b); break;
                case '-': eval.push(a - b); break;
                case '*': eval.push(a * b); break;
                case '/': eval.push(a / b); break;
                case '%': eval.push(fmod(a, b)); break;
                case '^': eval.push(pow(a, b)); break;
            }
        }
    }

    return eval.top();
}


std::unordered_map<std::string, int> int_vars;
std::unordered_map<std::string, std::string> string_vars;
std::unordered_map<std::string, float> float_vars;
std::unordered_map<std::string, long> long_vars;

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
        std::vector<std::string> tokens;
        size_t pos = 0;
        std::string token;
        std::string str = s;
        while ((pos = str.find(delimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            tokens.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        tokens.push_back(str);
        return tokens;
}

std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(s[start])) {
        ++start;
    }

    size_t end = s.size();
    while (end > start && std::isspace(s[end - 1])) {
        --end;
    }

    return s.substr(start, end - start);
}

int valueInVector(const std::vector<std::string>& arr, std::string val){
    // -1 if not found
    auto finder = std::find(arr.begin(), arr.end(), val);
    if (finder != arr.end()){
        return std::distance(arr.begin(), finder);
    }
    return -1;
}

int valueInArray(std::string arr[], int size, std::string val){
    // -1 if not found
    std::string* finder = std::find(arr, arr + size, val);
    if (finder != (arr + size)){
        return std::distance(arr, finder);
    }
    return -1;
}

bool valueInString(std::string str, char val){
    if (str.find(val) != std::string::npos){
        return true;
    }
    return false;
}

std::string isVar(std::string varName) {
    if (int_vars.find(varName) != int_vars.end()) return "int";
    if (string_vars.find(varName) != string_vars.end()) return "string";
    if (float_vars.find(varName) != float_vars.end()) return "float";
    if (long_vars.find(varName) != long_vars.end()) return "long";

    return "null";
}

bool isMathExpression(const std::string& input) {
    // Reject quoted strings
    if (input.front() == '"' && input.back() == '"') {
        return false;
    }

    // Match patterns like: 5+2, a + b * 3 - 1
    std::regex pattern(R"(^\s*[\w]+(\s*[\+\-\*/%]\s*[\w]+)+\s*$)");

    return std::regex_match(input, pattern);
}

std::string joinVectorElements(const std::vector<std::string>& vec, int start, int end) {
    if (start < 0) start = 0;
    if (end > vec.size()) end = vec.size();
    std::string result;
    for (int i = start; i < end; ++i) {
        result += vec[i];
    }
    return result;
}

std::string char_to_string(char c){
    std::string s(1, c);
    return s;
}

double handleMath(std::string eq){
    std::string symbols[5] = {"+", "-", "/", "*", "%"};
    std::vector<char> eq_splitted;
    for(int i = 0; i < eq.size(); i++){
        eq_splitted.push_back(eq[i]);
    }
    std::string new_eq = "";
    for(int k = 0; k < eq_splitted.size(); k++){
        if(valueInArray(symbols, sizeof(symbols)/sizeof(symbols[0]), char_to_string(eq_splitted[k]))){
            //for(int j = 0; j < k)
        }
    }
}

int main(){
    std::ifstream file("main.txt");

    // all variable types
    std::string variable_types[4] = {"int", "string", "float", "long"};
    std::string funcs[1] = {"print"};
    if (file.is_open()) {
        std::string line;
        int line_number = 1;
        while (getline(file, line)) {
            line = trim(line);
            // find variables if any in line
            std::vector<std::string> splitted_line = split(line, " ");
            int size = splitted_line.size();
            int var_val = valueInArray(variable_types, sizeof(variable_types) / sizeof(variable_types[0]),splitted_line[0]);
            if (var_val > -1){
                if (splitted_line[2] != "="){
                    std::cout << "Variable type declared but value not declared with = on line " << line_number << std::endl;
                    return 0;
                }
                std::string variable_name = splitted_line[1];
                std::string joinedVarVal = joinVectorElements(splitted_line, 3, size);
                if (size < 4){
                    std::cout << "Unexpected variable declaration on line " << line_number << std::endl;
                }
                if (var_val == 0 || var_val == 2 || var_val == 3){
                    // number
                    if(valueInString(splitted_line[3], '"')){
                        std::cout << "Double quotes found in number declaration on line " << line_number << std::endl;
                        return 0;
                    }
                    std::cout << '"' << joinedVarVal << '"' << "\n";
                    if(isMathExpression(joinedVarVal)){

                        splitted_line[3] = std::to_string(evaluateExpression(joinedVarVal));
                    }
                    if(size > 4){
                        std::cout << "Number declaration error on line " << line_number << std::endl;
                        return 0;
                    }
                    try {
                        if (var_val == 0){
                            // int
                            int_vars[variable_name] = stoi(splitted_line[3]);
                        } else if (var_val == 2){
                            //float
                            if(split(splitted_line[3], ".").size() > 1){
                                std::cout << "Float declared with multiple decimal points on line " << line_number << std::endl;
                                return 0;
                            }
                            float_vars[variable_name] = std::stof(splitted_line[3]);
                        } else if (var_val == 3){
                            //long
                            if(split(splitted_line[3], ".").size() > 1){
                                std::cout << "Long declared with multiple decimal points on line " << line_number << std::endl;
                                return 0;
                            }
                            long_vars[variable_name] = std::stol(splitted_line[3]);
                        }
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid Number argument on line " << line_number << std::endl;
                        return 0;
                    } catch (const std::out_of_range& e) {
                        std::cerr << "Out of range Number argument on line " << line_number << std::endl;
                        return 0;
                    }
                } else if(var_val == 1){
                    // string
                    std::string first_el = splitted_line[3];
                    std::string last_el = splitted_line.back();
                    if (first_el[0] != '"' || last_el[last_el.length() - 1] != '"'){
                        std::cout << "Missing double-quotes on line " << line_number << std::endl;
                        return 0;
                    }
                    std::string string_val = "";
                    for(int i = 3; i < size; i++){
                        string_val.append(splitted_line[i] + " ");
                    }
                    std::string new_string_val;
                    for(size_t k = 1; k < string_val.length() - 2; k++){
                        new_string_val += string_val[k];
                    }
                    string_vars[variable_name] = new_string_val;
                }
                continue;
            }
            // check for functions
            std::vector<std::string> left_paren_split = split(line, "(");
            std::vector<std::string> right_paren_split = split(line, ")");
            if( 
                splitted_line[0] == line && 
                left_paren_split[0] != line && 
                right_paren_split[0] != line && 
                left_paren_split.back().back() == ')' && 
                split(right_paren_split[0], left_paren_split[0])[1][0] == '('
            )
            {
                std::string func_name = left_paren_split[0];
                int func_val_in_arr = valueInArray(funcs, sizeof(funcs)/sizeof(funcs[0]), func_name);
                if(func_val_in_arr > -1){
                   if(func_name == "print"){
                    left_paren_split[1].pop_back();
                    std::string inside_func = left_paren_split[1];
                    if ((inside_func[0] == '"' && inside_func.back() != '"') || (inside_func.back() == '"' && inside_func[0] != '"')){
                        std::cout << "Missing double quote inside print " << " on line " << line_number << std::endl;
                        return 0;
                    } else if(inside_func[0] == '"' && inside_func.back() == '"'){
                        inside_func.erase(0, 1);
                        inside_func.pop_back();
                        std::cout << inside_func;
                    } else {
                        // variable
                        std::string var_val = isVar(inside_func);
                        if (var_val == "null") {
                            std::cout << "Unknown variable in print on line " << line_number << std::endl;
                            return 0;
                        } else if (var_val == "string") {
                            std::cout << string_vars[inside_func];
                        } else if (var_val == "int") {
                            std::cout << int_vars[inside_func];
                        } else if (var_val == "float") {
                            std::cout << float_vars[inside_func];
                        } else if (var_val == "long") {
                            std::cout << long_vars[inside_func];
                        }
                    }
                   }
                }
            }
            line_number++;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file" << std::endl;
    }

    return 0;
}