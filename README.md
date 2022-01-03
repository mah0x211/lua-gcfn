lua-gcfn
====

[![test](https://github.com/mah0x211/lua-gcfn/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-gcfn/actions/workflows/test.yml)

gcfn create the finalizer object.


## Installation

```
luarocks install gcfn
```

## Usage

### gco = gcfn.new( fn [, ...] )

create a new gc fucntion object.

**Parameters**

- `fn:function`: a function that run in coroutine when GC occurs.
- `...: `: arguments of finalize function

**Returns**

- `gco:gcfn`: gcfn object.


**Example**

```lua
local gcfn = require('gcfn')

-- create new gcfn object
local gco = gcfn.new(function(...)
    print(...)
end, 'a', 'b', 'c' )

gco = nil
collectgarbage('collect')
print('done')
```

### gco:disable()

disable the invocation of GC function.

### gco:enable()

enable the invocation of GC function.

