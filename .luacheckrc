std = 'max'
include_files = {
    'test/*_test.lua',
}
ignore = {
    'assert',
    -- Value assigned to a local variable is unused
    '311',
}
