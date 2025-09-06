// Hello.td - A tiny TableGen example
//
// ❯ llvm-tblgen ./hello.td
// ------------- Classes -----------------
// class Person<string Person:name = ?, int Person:age = ?> {
//   string Name = Person:name;
//   int Age = Person:age;
// }
// ------------- Defs -----------------
// def Alice {	// Person
//   string Name = "Alice";
//   int Age = 33;
// }
// def Bob {	// Person
//   string Name = "Bob";
//   int Age = 30;
// }
//
// ------------------------------------------
// Base "class"
class Person<string name, int age> {
    string Name = name;
    int Age = age;
}

// Define some "records" i.e., instances of Person
def Alice : Person<"Alice", 33>;
def Bob   : Person<"Bob", 30>;

