#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace AST {
	union Token {
		double fractional;
		int64_t integer;
		struct {
			int32_t priority;
			char val;
		} sign;
	};

	const std::unordered_map<char, int> signToPriority = {
		{'+', 1}, {'-', 1}, {'*', 2}, {'^', 3}, {'/', 2}, {'|', 2}, {'%', 2} };

	const std::unordered_map<char, double> constants = { {'e', 2.7}, {'p', 3.4} };

	const std::int32_t adder =
		std::max_element(signToPriority.begin(), signToPriority.end(),
			[](auto l, auto r) { return l.second < r.second; })->second + 1;

	const std::size_t end = std::numeric_limits<std::size_t>::max();


	std::vector<Token> Tokenize(std::string &str) {
		std::vector<Token> result;

		std::size_t numberStart = end; // presumably you'll run out of memory the way sooner
		std::size_t numberEnd = end;

		std::int32_t lvl = 0;
		auto it = signToPriority.end();

		for (std::size_t i = 0; i <= str.size(); ++i) { // string.c_str() - null-terminated array of char
			if (std::isdigit(str[i])) {
				if (numberStart == end)
					numberStart = numberEnd = i;
				else
					++numberEnd;
			}
			else if (numberEnd != end) {
				if (result.size() % 2 != 0)
					throw std::exception("More than one sign in a row.");

				Token t;

				t.integer = std::stoi(std::string(str, numberStart, numberEnd + 1));

				result.push_back(t);
				numberStart = numberEnd = end;
			}

			if (str[i] == '(')
				++lvl;
			else if (str[i] == ')') {
				--lvl;
				if (lvl < 0)
					throw std::exception("Brackets do not match");
			}

			auto it = signToPriority.find(str[i]);

			if (it != signToPriority.end()) {
				if (result.size() % 2 == 0)
					throw std::exception("More than one sign in a row.");

				Token t;

				t.sign.priority = lvl * adder + signToPriority.find(str[i])->second;
				t.sign.val = str[i];

				result.push_back(t);
			}
		}

		if (lvl != 0)
			throw std::exception("Brackets do not match");

		return result;
	};

	template <class T> T eval(char ch, T &lhs, T &rhs) {
		static_assert(std::is_integral<T>::value, "Integral required.");
		static_assert(std::is_floating_point<T>::value, "Floating point required.");

		switch (ch) {
		case '+':
			return lhs + rhs;
		case '-':
			return lhs - rhs;
		case '*':
			return lhs * rhs;
		case '/':
			return lhs / rhs;
		case '^':
			return std::pow(lhs, rhs);
		case '|':
			return 0; // check later
		case '%':
			return 0; // check later

		default:
			throw std::exception("Wrong operator: " + ch);
		}
	};
}; // namespace AST

int main() {
	std::string s = "1 * (23 / 123 ^ 1) + (123 + 23 + 1) + 3";
	std::cout << s << std::endl << std::endl;

	// std::cin >> s;

	try {
		auto res = AST::Tokenize(s);

		bool even = true;

		for (auto &t : res) {
			if (even)
				std::cout << "Number: " << t.integer;
			else {
				std::cout << "Sign: " << t.sign.val
					<< ", priority: " << t.sign.priority;
			}

			even = !even;

			std::cout << std::endl;
		}
	}
	catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}

	std::cin.get();

	return 0;
}
