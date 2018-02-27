test_units = [
  'test-completion',
  'test-link',
  'test-search-context',
  'test-util'
]

foreach test_unit: test_units
  exe = executable(
    test_unit,
    test_unit + '.c',
    include_directories: top_inc,
    dependencies: libdevhelp_core_dep
  )

  test(test_unit, exe)
endforeach
