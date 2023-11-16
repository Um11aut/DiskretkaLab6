#include <iostream>
#include <vector>
#include <map>

class Function {
private:
    constexpr friend std::ostream& operator<<(std::ostream& os, const Function& other) {
        os << other.name;
        return os;
    }
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
    static void unify(Function& t1, Function& t2) {
        for (size_t i = 0; i < t1.arguments.size(); ++i) {
            std::cout << t1.arguments[i] << " = " << t2.arguments[i] << std::endl;
        }
    }
};
