lua-gcfn
====

gcfn create the finalizer object.


## Installation

```
luarocks install gcfn --from=http://mah0x211.github.io/rocks/
```

## Create a Finalizer

### f = gcfn.new( fn [, ...] )

create a new finalizer object.

**Parameters**

- `fn:function`: finalize function that run in coroutine.
- `...: `: arguments of finalize function


**Example**

```lua
local gcfn = require('gcfn')

-- create finalizer
local f = gcfn.new(function(...)
    print(...)
end, 'a', 'b', 'c' )

print('done')
```

**Returns**

1. `f:userdata`: finalizer object.


## Finalizer Methods

### f:disable()

disabling the finalizer.

### f:enable()

dnabling the finalizer.

