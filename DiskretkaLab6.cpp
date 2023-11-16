#include "disjunct.h"

int main() {
	Clause p('p');
	Clause q('q');
	Clause s('s');

	Clause r('r');

	std::vector<ClauseExpression> clauseExpressions;

	clauseExpressions.push_back(ClauseExpression({ p,q,s }, { Operator::OR, Operator::OR }));
	clauseExpressions.push_back(ClauseExpression({ p,r }, { Operator::IMPLICATION }));
	clauseExpressions.push_back(ClauseExpression({ q,r }, { Operator::IMPLICATION }));
	clauseExpressions.push_back(ClauseExpression({ s,r }, { Operator::IMPLICATION }));

	for (size_t i = 0; i < clauseExpressions.size(); ++i) {
		std::cout << clauseExpressions[i] << std::endl;
	}
	std::cout << std::endl;

	std::vector<std::vector<Segment>> simplifiedExpressions;

	for (size_t i = 0; i < clauseExpressions.size(); ++i) {
		if (!clauseExpressions[i].simplify().empty()) {
			simplifiedExpressions.push_back(clauseExpressions[i].simplify());
		}
	}

	for (const auto& expr : simplifiedExpressions) {
		for (const auto& segment : expr) {
			segment.print();
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;


	ClauseExpression::prove(simplifiedExpressions, { {r} });

	std::cout << std::endl;
}