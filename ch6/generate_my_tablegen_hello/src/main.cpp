#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifndef TBLGEN_JSON_PATH
#define TBLGEN_JSON_PATH "generated/hello.json"
#endif

struct Person {
  std::string name;
  int age{};
};

int main() {
  // Prefer the path passed by CMake; fall back to local copy if needed.
  std::vector<std::string> candidates = {
      TBLGEN_JSON_PATH, "hello.json", "generated/hello.json"};

  std::ifstream in;
  std::string usedPath;
  for (const auto &p : candidates) {
    in.open(p);
    if (in.is_open()) { usedPath = p; break; }
  }

  if (!in.is_open()) {
    std::cerr << "ERROR: Could not open generated JSON. Tried:\n";
    for (const auto &p : candidates) std::cerr << "  - " << p << "\n";
    std::cerr << "Did you run the build (so llvm-tblgen generates it)?\n";
    return 1;
  }

  std::ostringstream buf;
  buf << in.rdbuf();
  const std::string json = buf.str();
  in.close();

  // Minimal regex-based "parse" tailored to TableGen -dump-json output:
  // Extract record blocks containing "name": "<Name>" and fields Age/Name.
  // Portable version (works in C++17 too)

  const std::regex personRe(
        R"REGEX("([A-Za-z0-9_]+)"\s*:\s*\{[^{}]*"!superclasses"\s*:\s*\[\s*"Person"\s*\][^{}]*"Age"\s*:\s*([0-9]+)[^{}]*"Name"\s*:\s*"([^"]+)"[^{}]*\})REGEX",
        std::regex::ECMAScript
    );


  std::vector<Person> people;
  for (auto it = std::sregex_iterator(json.begin(), json.end(), personRe);
       it != std::sregex_iterator(); ++it) {
    std::smatch m = *it;
    // m[1] is the record name (e.g., "Alice"), m[2] is fields.Name, m[3] is fields.Age
    Person p;
    p.name = m[3].str();  // use the fields.Name as the display name
    p.age = std::stoi(m[2].str());
    people.push_back(p);
  }

  std::cout << "Loaded " << people.size() << " record(s) from: " << usedPath << "\n";
  for (const auto &p : people) {
    std::cout << "- " << p.name << " (Age=" << p.age << ")\n";
  }
  return 0;
}
