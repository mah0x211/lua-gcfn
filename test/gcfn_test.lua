local testcase = require('testcase')
local gcfn = require('gcfn')

function testcase.create_gcfn()
    -- test that create gcfn object
    local gco = assert(gcfn(function()
    end))
    assert.match(tostring(gco), '^gcfn: ', false)

    -- test that throws an erro if 1st argument is not function
    local err = assert.throws(gcfn, {})
    assert.match(err, '#1 .+ [(]function expected,', false)
end

function testcase.call_on_gc()
    local called = false
    local fn = function()
        called = true
    end

    -- test that the gc function called on gc
    assert(gcfn(fn))
    collectgarbage('collect')
    assert.is_true(called)
end

function testcase.disable_call()
    local called = false
    local fn = function()
        called = true
    end

    -- test that gc function does not called on gc if sets it disabled
    local gco = assert(gcfn(fn))
    gco:disable()
    gco = nil
    collectgarbage('collect')
    assert.is_false(called)
end

function testcase.enable_call()
    local called = false
    local fn = function()
        called = true
    end

    -- test that gc function called on gc if sets it enabled
    local gco = assert(gcfn(fn))
    gco:disable()
    gco:enable()
    gco = nil
    collectgarbage('collect')
    assert.is_true(called)
end

function testcase.throw_error_on_gc()
    local fn = function()
        error('throws an error')
    end

    -- test that the gc function called on gc
    assert(gcfn(fn))
    collectgarbage('collect')
end

