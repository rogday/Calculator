#include <vector>
#include <regex>
#include <string>
#include <iostream>
#include <cctype>
#include <unordered_map>
#include <stack>
#include <functional>
#include <cmath>

struct Func {
	int priority;
	int arity;
	std::function<float(std::vector<float>&)> func;
};

std::unordered_map<std::string, Func> signToFunc = {
		{"+", {1, 2, [](std::vector<float>& a) {return a[0] + a[1]; }}},
		{"-", {1, 2, [](std::vector<float>& a) {return a[0] - a[1]; }}},
		{"*", {2, 2, [](std::vector<float>& a) {return a[0] * a[1]; }}},
		{"/", {2, 2, [](std::vector<float>& a) {return a[0] / a[1]; }}},
		{"^", {3, 2, [](std::vector<float>& a) {return pow(a[0], a[1]); }}},

		{"ln", {3, 1, [](std::vector<float>& a) {return log(a[0]); }}},
		{"lg", {3, 1, [](std::vector<float>& a) {return log(a[0]) / log(10); }}},
		{"log", {3, 2, [](std::vector<float>& a) {return log(a[0]) / log(a[1]); }}},

		{"sqrt", {3, 1, [](std::vector<float>& a) {return pow(a[0], 0.5); }}},

		{"(", {4, 0, [](std::vector<float>& a) {return 42; }}},
		{")", {0, 0, [](std::vector<float>& a) {return 42; }}},
		{"=", {-1, 0, [](std::vector<float>& a) {return 42; }}}
};

std::unordered_map<std::string, float> consts = { {"e", 2.7}, {"pi", 3.1}, {"g", 9.8} };

int main() {
	std::string pattern = R"((\d+(\.\d+)?)|(\=)";

	for (auto& p : signToFunc) {
		if (!isalpha(p.first[0]))
			pattern += "|\\" + p.first;
		else
			pattern += "|" + p.first;
	}

	for (auto& p : consts)
		pattern += "|" + p.first;
	pattern += ")";

	std::regex num(pattern);
	std::string string;
	std::getline(std::cin, string);
	string += "=";

	auto start = std::sregex_iterator(string.begin(), string.end(), num);
	auto end = std::sregex_iterator();

	std::stack<std::string> functions;
	std::stack<float> numbers;

	bool isPrevNum = false;

	for (auto i = start; i != end; ++i) {
		auto& substr = i->str();
		auto it = consts.find(substr);

		if (it != consts.end()) {
			numbers.push(it->second);
			isPrevNum = true;
		} else {
			if (isdigit(substr[0])) {
				numbers.push(stof(substr));
				isPrevNum = true;
			} else {
				while (!functions.empty() && (signToFunc[substr].priority <= signToFunc[functions.top()].priority) &&
					(numbers.size() >= signToFunc[functions.top()].arity)) {
					Func function = signToFunc[functions.top()];

					if (functions.top() == "(")
						break;

					std::vector<float> args(function.arity);

					std::cout << "aplying " << functions.top() << ", args: "<<std::endl;
					for (int i = function.arity - 1; i >= 0; --i) {
						args[i] = numbers.top();
						std::cout << args[i] << ' ';
						numbers.pop();
					}
					std::cout <<std::endl;

					numbers.push(function.func(args));
					functions.pop();
				}

				if (!functions.empty() && functions.top() == "(" && substr == ")")
					functions.pop();
				else {
					functions.push(substr);
					if (substr == "-" && !isPrevNum)
						numbers.push(0);
				}
				isPrevNum = false;
			}
		}
	}
	
	if (numbers.size() != 1 || functions.size() != 1 || functions.top() != "=")
		std::cout << "error." << std::endl;
	else
		std::cout << numbers.top() << std::endl;

	std::cin.get();
}
