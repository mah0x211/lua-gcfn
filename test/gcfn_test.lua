local testcase = require('testcase')
local gcfn = require('gcfn')

function testcase.test_all_method()
    local called = false
    local fn = function()
        called = true
    end

    -- test that create gcfn object
    local gco = assert(gcfn(fn))
    assert.match(tostring(gco), '^gcfn: ', false)

    -- test that the gc function called on gc
    gco = nil
    collectgarbage('collect')
    assert.is_true(called)

    -- test that gc function does not called on gc if sets it disabled
    called = false
    gco = assert(gcfn(fn))
    gco:disable()
    gco = nil
    collectgarbage('collect')
    assert.is_false(called)

    -- test that gc function called on gc if sets it enabled
    called = false
    gco = assert(gcfn(fn))
    gco:disable()
    gco:enable()
    gco = nil
    collectgarbage('collect')
    assert.is_true(called)
end
