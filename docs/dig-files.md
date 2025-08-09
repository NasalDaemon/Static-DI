# Cluster and trait `.dig` file template

## Header includes and module imports

At the top of `.hxx.dig` header files, include the necessary headers. No include guard is necessary.

```cpp
// fruit.hxx.dig
#include "my/apple.hpp"
#include "my/pear.hpp"

namespace my {
    // ...
}
```

At the top of `.ixx.dig` module files, specify all module imports at the top of the file, as well as including any necessary headers in the global module fragment.

```cpp
// fruit.ixx.dig
export module my.fruit;

import my.apple;
import my.pear;

namespace my {
    // ...
}
```
<details>
<summary>With headers in global module fragment</summary>

```cpp
// fruit.ixx.dig
module;
#include "my/apple.hpp"
export module my.fruit;

import my.pear;

namespace my {
    // ...
}
```
</details>

Underneath the headers and module imports, the syntax supported by `.hxx.dig` and `.ixx.dig` is identical.
