lua-gcfn
====

[![test](https://github.com/mah0x211/lua-gcfn/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-gcfn/actions/workflows/test.yml)
[![codecov](https://codecov.io/gh/mah0x211/lua-gcfn/branch/master/graph/badge.svg)](https://codecov.io/gh/mah0x211/lua-gcfn)

gcfn create the finalizer object.


## Installation

```
luarocks install gcfn
```

## Usage

```lua
local gcfn = require('gcfn')

-- create new gcfn object
local gco = gcfn(function(...)
    print(...)
end, 'a', 'b', 'c' )

gco = nil
collectgarbage('collect')
print('done')
```


## gco = gcfn( fn [, ...] )

create a new gc fucntion object.

**Parameters**

- `fn:function`: a function to execute when GC occurs.
- `...: `: arguments of function

**Returns**

- `gco:gcfn`: gcfn object.


## gco:disable()

disable the invocation of GC function.


## gco:enable()

enable the invocation of GC function.


## gco:errfunc( fn, ... )

sets the error function to be called when the GC function fails.

**Parameters**

- `fn:function`: a function.
- `...: `: arguments of function.

**NOTE**

The error object `err` is passed afterthe the arguments `...`.

