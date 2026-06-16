local testcase = require('testcase')
local assert = require('assert')
local gcfn = require('gcfn')

function testcase.create_gcfn()
    -- test that create gcfn object
    local gco = assert(gcfn(function()
    end))
    assert.match(tostring(gco), '^gcfn: ', false)

    -- test that throws an error if 1st argument is not function
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
    -- test that errfn will be called when gc function fails
    local err
    local errfn = function(...)
        err = {
            ...,
        }
    end
    local gco = assert(gcfn(function()
        error('throws an error')
    end))
    gco:errfunc(errfn, 'foo', 'bar')
    gco = nil
    collectgarbage('collect')
    -- confirm that errfn is called with arguments
    assert.equal(err[1], 'foo')
    assert.equal(err[2], 'bar')
    assert.match(err[3], 'throws an error')
end

function testcase.remove_errfn()
    local gco = assert(gcfn(function()
        error('throws an error')
    end))
    local err
    gco:errfunc(function(...)
        err = {
            ...,
        }
    end)

    -- test that remove errfn
    gco:errfunc()
    gco = nil
    collectgarbage('collect')
    assert.is_nil(err)
end

function testcase.throw_error_if_1st_arg_is_not_function()
    -- test that throws an error if 1st argument is not function
    local gco = assert(gcfn(function()end))
    local err = assert.throws(gco.errfunc, gco, {})
    assert.match(err, '#2 .+ [(]function expected,', false)
end

