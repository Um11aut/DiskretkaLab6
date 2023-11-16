#include <iostream>
#include <vector>
#include <map>

class Function {
public:
    std::string name;
    std::vector<Function> arguments;

    Function(std::string n, std::vector<Function> args) : name(n), arguments(args) {}

    void print() const {
        std::cout << name;
        if (!arguments.empty()) {
            std::cout << "(";
            for (const auto& arg : arguments) {
                arg.print();
                if (&arg != &arguments.back()) {
                    std::cout << ", ";
                }
            }
            std::cout << ")";
        }
    }
};

class Unifier {
public:
    static bool unify(const Function& t1, const Function& t2, std::map<std::string, Function>& substitution) {
        if (t1.name != t2.name || t1.arguments.size() != t2.arguments.size()) {
            return false;
        }

        for (size_t i = 0; i < t1.arguments.size(); ++i) {
            if (!unify(t1.arguments[i], t2.arguments[i], substitution)) {
                return false;
            }
        }

        return true;
    }
};
