// tablegen/Hello.td - A tiny TableGen example showing data -> JSON
// Define a simple base class.
class Person<string name, int age> {
  string Name = name;
  int Age = age;
}

// Four records (instances).
def Alice : Person<"Alice", 30>;
def Bob   : Person<"Bob", 40>;
def John  : Person<"John", 15>;
def Steve : Person<"Steve", 78>;
