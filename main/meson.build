project(
  'Aeris main',
  'cpp',
  default_options: ['cpp_std=c++11'],
  license: 'MIT'
)

thread_dep = dependency('threads')

add_global_arguments('-Wextra', '-Wpedantic', '-Wno-expansion-to-defined', language : 'cpp')

global_inc = include_directories('src')

subdir('src')
