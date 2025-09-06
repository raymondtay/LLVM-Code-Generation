
class MyClass<string _alias=""> {
    string alias = _alias;
}

let alias = "let from out" in
def A : MyClass<> {} // the 'alias' does not spill over to 'B'
def B : MyClass<> {} {
    let alias = "let from body";
}

def C : MyClass<"from arg"> ;
let alias = "alias frmo bigger scope" in {
    let alias = "let from out" in 
    def D : MyClass<"from arg"> {
        let alias = "let from body"; // overrides the outerscope variable
    }

    def E : MyClass<"will be overridden">;
}
