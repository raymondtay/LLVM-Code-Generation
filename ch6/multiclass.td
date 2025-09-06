// ❯ llvm-tblgen ./multiclass.td
// ------------- Classes -----------------
// class MetaInformation<string MetaInformation:keyName = ?> {
//   string Key = MetaInformation:keyName;
// }
// class ShippingPrice<int ShippingPrice:arg = ?> {
//   int shippingPrice = ShippingPrice:arg;
// }
// ------------- Defs -----------------
// def valuedBundleA {	// ShippingPrice MetaInformation
//   string name = "valued-A";
//   int price = 12;
//   int weight = 1;
//   int shippingPrice = 5;
//   string Key = "metadata";
// }
// def valuedBundleB {	// ShippingPrice MetaInformation
//   string name = "valued-B";
//   string tag = "special";
//   int shippingPrice = 5;
//   string Key = "metadata";
// }
// def valuedBundleC {	// ShippingPrice MetaInformation
//   string name = "valued-C";
//   string tag = "special concoction";
//   int shippingPrice = 5;
//   string Key = "metadata";
// }
// 
multiclass Bundle<string base> {
  def A {
    string name = !strconcat(base, "-", "A");
    int price = 12;
    int weight = 1;
  }
  def B {
    string name = !strconcat(base, "-", "B");
    string tag = "special";
  }
  def C {
    string name = !strconcat(base, "-", "C");
    string tag = "special concoction";
  }
}

class ShippingPrice<int arg> {
  int shippingPrice = arg;
}

class MetaInformation<string keyName> {
    string Key = keyName;
}

defm valuedBundle : Bundle<"valued">, ShippingPrice<5>, MetaInformation<"metadata">;

