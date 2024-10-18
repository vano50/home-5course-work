#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <map>

class IniParserException : public std::runtime_error {
public:
    explicit IniParserException(const std::string& message) : runtime_error(message) {}
};

class IniParser {
private:
    std::map<std::string, std::map<std::string, std::string>> sections;

public:
    void parseFile(const std::string& filepath) {
        std::ifstream input(filepath);
        if (!input.is_open()) {
            throw IniParserException("Нету файла");
        }

        std::string line;
        std::string currentSection;

        while (getline(input, line)) {
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

            if (line.empty() || line[0] == '#') {
                continue;
            }

            if (line.front() == '[' && line.back() == ']') {
                currentSection = line.substr(1, line.size() - 2);
            }
            else {
                size_t pos = line.find('=');
                if (pos != std::string::npos && !currentSection.empty()) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    key.erase(remove_if(key.begin(), key.end(), isspace), key.end());
                    value.erase(remove_if(value.begin(), value.end(), isspace), value.end());
                    sections[currentSection][key] = value;
                }
            }
        }
    }

    template <typename T>
    T getValue(const std::string& sectionName, const std::string& key) const {
        auto it = sections.find(sectionName);
        if (it == sections.end()) {
            throw IniParserException("Нету секции");
        }
        auto it2 = it->second.find(key);
        if (it2 == it->second.end()) {
            throw IniParserException("Нету ключа");
        }

        if constexpr (std::is_same<T, std::string>::value) {
            return it2->second;
        }
        else if constexpr (std::is_same<T, int>::value) {
            return std::stoi(it2->second);
        }
        else {
            throw IniParserException("Не поддерживаемый тип");
        }
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    try {
        IniParser parser;
        IniParser parser2;
        parser.parseFile("config.ini");
        parser2.parseFile("config2.ini");

        int myInt = parser.getValue<int>("Section1", "MyIntVariable");
        std::string myString = parser.getValue<std::string>("Section1", "MyStringVariable");
        int myInt2 = parser.getValue<int>("Section2", "AnotherIntVariable");
        std::string myString2 = parser.getValue<std::string>("Section2", "AnotherStringVariable");

        int myInt3 = parser2.getValue<int>("Section1", "MyIntVariable");
        std::string myString3 = parser2.getValue<std::string>("Section1", "MyStringVariable");
        int myInt4 = parser2.getValue<int>("Section2", "AnotherIntVariable");
        std::string myString4 = parser2.getValue<std::string>("Section2", "AnotherStringVariable");

        std::cout << "myInt = " << myInt << std::endl;
        std::cout << "myString = " << myString << std::endl;
        std::cout << "myInt2 = " << myInt2 << std::endl;
        std::cout << "myString2 = " << myString2 << std::endl;

        std::cout << "myInt3 = " << myInt3 << std::endl;
        std::cout << "myString3 = " << myString3 << std::endl;
        std::cout << "myInt4 = " << myInt4 << std::endl;
        std::cout << "myString4 = " << myString4 << std::endl;
    }
    catch (IniParserException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}