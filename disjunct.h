#pragma once
#include <optional>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <map>

class Clause
{
private:
	std::optional<char> m_name = std::nullopt;
	std::optional<std::string> m_nameString = std::nullopt;
	bool m_negation = false;

public:
	Clause(const char name) : m_name(name) {}
	Clause(const char name, const bool negation) : m_name(name), m_negation(negation) {}

	void operator--() {
		m_negation = !m_negation;

		if (m_negation) {
			std::string temp;
			
			std::stringstream ss;
			ss << '-' << m_name.value();
			ss >> temp;

			m_nameString = temp;
		}
		else {
			m_name = std::nullopt;
		}
	}

	const bool getNegation()const {
		return m_negation;
	}

	const bool operator==(const Clause& other) const {
		if (m_name == other.m_name) {
			return true;
		}
		if (other.m_nameString.has_value()) {
			if (m_name == other.m_nameString.value()[1]) return true;
		}
		if (m_nameString.has_value()) {
			if (m_nameString.value()[1] == other.m_name) return true;
		}
		return false;
	}

	const bool operator!=(const Clause& other) const {
		if (m_name == other.m_name) {
			return false;
		}
		if (other.m_nameString.has_value()) {
			if (m_name == other.m_nameString.value()[1]) return false;
		}
		if (m_nameString.has_value()) {
			if (m_nameString.value()[1] == other.m_name) return false;
		}
		return true;
	}

	Clause operator-() const&
	{
		Clause result = *this;
		result.m_negation = !result.m_negation;
		if (result.m_negation) {
			std::string temp;
			std::stringstream ss;

			ss << '-' << result.m_name.value();
			ss >> temp;

			result.m_nameString = temp;
			result.m_name = std::nullopt;
		}
		else {
			result.m_nameString = std::nullopt;
		}
		return result;
	}

	void operator=(const Clause& other)
	{
		m_name = other.m_name;
		m_nameString = other.m_nameString;
		m_negation = other.m_negation;
	}

	constexpr friend std::ostream& operator<<(std::ostream& other, const Clause& obj)
	{
		if (obj.m_nameString.has_value())
		{
			other << obj.m_nameString.value();
		}
		else {
			other << obj.m_name.value();
		}
		return other;
	}
};


enum Operator {
	AND, // Ʌ
	IMPLICATION, // ->
	OR // V
};

static const std::string operatorConverter(Operator el) {
	switch (el) {
	case (Operator::AND):
		return "Ʌ";
	case (Operator::OR):
		return "V";
	case (Operator::IMPLICATION):
		return "->";
	default:
		return "";
	}
}

struct Segment {
	std::vector<Clause> m_clauses;
	std::vector<Operator> m_operators;
	bool m_negation = false;
	bool rvalue = false;

	Segment(std::vector<Clause>& clauses, std::vector<Operator>& operators)
		: m_clauses(clauses), m_operators(operators) {}
	Segment(std::vector<Clause> clauses)
		: m_clauses(clauses) {}
	Segment() {}

	void operator()(Segment&& other) noexcept {
		m_negation = other.m_negation;
		rvalue = other.rvalue;
		
		m_clauses.clear();
		m_clauses.insert(m_clauses.begin(), other.m_clauses.begin(), other.m_clauses.end());

		m_operators.clear();
		m_operators.insert(m_operators.begin(), other.m_operators.begin(), other.m_operators.end());

		other.m_clauses.clear();
		other.m_operators.clear();
	}

	void operator--() {
		m_negation = !m_negation;
	}

	bool operator==(const Segment& other) {
		if (m_clauses.size() != other.m_clauses.size()) return false;

		for (size_t i = 0; i < m_clauses.size(); ++i) {
			if (m_clauses[i] != other.m_clauses[i]) {
				return false;
			}
		}
		return true;
	}

	void insertStart(Segment segment) {
		m_clauses.insert(m_clauses.begin(), segment.m_clauses.begin(), segment.m_clauses.end());
	}

	void remove(const std::vector<Clause>& clausesToRemove) {
		for (const auto& clause : clausesToRemove) {
			std::erase(m_clauses, clause);
		}
	}

	void convert(bool rvalue) {
		if (!m_operators.empty()) {
			if (m_operators[0] == Operator::AND) {
				for (size_t i = 0; i < m_clauses.size(); ++i) {
					--m_clauses[i];
					if (i >= 0 && m_operators.size() >= i + 1)
						m_operators[i] = Operator::OR;
				}
				this->operator--();
			}
			if (m_operators[0] == Operator::IMPLICATION) {
				for (size_t i = 0; i < m_operators.size(); ++i) {
					m_operators[i] = Operator::OR;
				}
			}
			else {
				return;
			}
		}
		this->rvalue = rvalue;
	}

	Operator getOperator() const {
		auto it = std::adjacent_find(m_operators.begin(), m_operators.end(), std::not_equal_to<int>());
		if (it == m_operators.end() && !m_operators.empty()) {
			return m_operators[0];
		}
	}

	void print() const {
		if (m_negation) {
			std::cout << "-(";
		}

		for (size_t i = 0; i < m_clauses.size(); ++i) {
			if (rvalue) {
				if (m_operators.size() >= i + 1)
					std::cout << ' ' << operatorConverter(m_operators[i]) << ' ';
				std::cout << m_clauses[i];
				break;
			}

			std::cout << m_clauses[i];
			if (m_operators.size() >= i + 1 && i < m_clauses.size() - 1)
				std::cout << ' ' << operatorConverter(m_operators[i]) << ' ';
		}

		if (m_negation) {
			std::cout << ")";
		}
	}
};

class ClauseExpression {
private:
	std::vector<Clause> m_clauses;
	std::vector<Operator> m_operators;

	bool m_negation = false;


	const std::vector<Segment> segmentDivider() const {
		std::vector<Segment> segments;

		for (size_t i = 0; i < m_operators.size(); ++i) {
			Segment temp{};

			if (m_operators[i] == Operator::AND) {
				temp.m_clauses = { m_clauses[i] };
				temp.m_operators = { m_operators[i] };

				while (i + 1 < m_operators.size() && m_operators[i] == m_operators[i + 1]) {
					temp.m_clauses.push_back(m_clauses[i + 1]);
					temp.m_operators.push_back(m_operators[i + 1]);

					++i;
				}
				temp.m_clauses.push_back(m_clauses[i + 1]);
				temp.convert(false);
			}
			else if (m_operators[i] == Operator::IMPLICATION) {
				bool tempConvert = false;
				if (segments.size() > 0) {
					--segments[segments.size() - 1];
					tempConvert = true;
				}
				else {
					temp.m_clauses.push_back(-m_clauses[i]);
					tempConvert = false;
				}

				temp.m_clauses.push_back(m_clauses[i + 1]);
				temp.m_operators = { m_operators[i] };

				temp.convert(tempConvert);
			}
			else if (m_operators[i] == Operator::OR) {
				temp.m_clauses.push_back(m_clauses[i]);

				while (i + 1 < m_operators.size() && m_operators[i] == m_operators[i + 1]) {
					temp.m_clauses.push_back(m_clauses[i + 1]);
					temp.m_operators.push_back(m_operators[i + 1]);

					++i;
				}
				temp.m_operators.push_back(Operator::OR);
				temp.m_clauses.push_back(m_clauses[i + 1]);
			}

			segments.push_back(temp);
		}

		return segments;
	}
public:
	
	ClauseExpression(std::vector<Clause> clauses, std::vector<Operator> operators) 
		: m_clauses(clauses), m_operators(operators)
	{
		if (operators.size() != clauses.size() - 1) throw std::invalid_argument("Invalid operator's vector size!");
	}

	void operator--() {
		m_negation = !m_negation;
	}

	constexpr friend std::ostream& operator<<(std::ostream& other, ClauseExpression& obj) {
		for (size_t i = 0; i < obj.m_clauses.size(); ++i) {
			other << obj.m_clauses[i];
			if (obj.m_operators.size() >= i + 1)
				other << ' ' << operatorConverter(obj.m_operators[i]) << ' ';
		}
		return other;
	}

	inline const std::pair<std::vector<Clause>, std::vector<Operator>> get()const& { return std::pair<std::vector<Clause>, std::vector<Operator>>(m_clauses, m_operators); }

	std::vector<Segment> simplify() {
		const auto& simplifiedSegments = segmentDivider();

		return simplifiedSegments;
	}

	static void prove(std::vector<std::vector<Segment>>& simplifiedSegments, Segment what) {
		for (size_t i = 1; i < simplifiedSegments.size(); ++i) {
			auto& first = simplifiedSegments[0];
			auto& second = simplifiedSegments[i];

			if (first[0].m_clauses.empty() || second[0].m_clauses.empty()) {
				continue;
			}

			std::cout << '(';
			first[0].print();
			std::cout << ") & (";
			second[0].print();
			std::cout << ')';
			std::cout << std::endl;

			std::pair<std::vector<Clause>, std::vector<Clause>> clausesToRemove;

			for (const auto& firstSegmentClause : first[0].m_clauses) {
				for (const auto& secondSegmentClause : second[0].m_clauses) {
					if (firstSegmentClause == secondSegmentClause) {
						if (firstSegmentClause.getNegation() != secondSegmentClause.getNegation()) {
							std::cout << firstSegmentClause << " та " << secondSegmentClause << " викреслюємо" << std::endl;
								
							auto element1 = std::find(first[0].m_clauses.begin(), first[0].m_clauses.end(), firstSegmentClause);
							auto element2 = std::find(second[0].m_clauses.begin(), second[0].m_clauses.end(), secondSegmentClause);
							
							clausesToRemove.first.push_back(*element1);
							clausesToRemove.second.push_back(*element2);

							if (firstSegmentClause == first[0].m_clauses.back() && secondSegmentClause == second[0].m_clauses.back()) {
								if (i + 1 <= simplifiedSegments.size()) {
									second[0](std::move(simplifiedSegments[i + 1][0]));
								}

								prove(simplifiedSegments, what);
							}
							else {
								continue;
							}

						}
						if(firstSegmentClause.getNegation() == secondSegmentClause.getNegation()) {
							auto element = std::find(first[0].m_clauses.begin(), first[0].m_clauses.end(), firstSegmentClause);
							
							clausesToRemove.first.push_back(*element);
							if (firstSegmentClause == first[0].m_clauses.back() && secondSegmentClause == second[0].m_clauses.back()) {
								std::cout << firstSegmentClause << " та " << secondSegmentClause << " додаємо" << std::endl;

								if (i + 1 <= simplifiedSegments.size()) {
									second[0](std::move(simplifiedSegments[i + 1][0]));
								}
								prove(simplifiedSegments, what);
							}
							else {
								continue;
							}
						}
					}
				}
			}

			first[0].remove(clausesToRemove.first);
			second[0].remove(clausesToRemove.second);

			first[0].insertStart(second[0]);

			if (first[0] == what) {
				std::cout << "=> ";
				what.print();
				std::cout << " - Доведено";
			}
		}
		std::cout << std::endl;
	}
};