project('Musical_Things', 'cpp', default_options: ['cpp_std=c++20'])

cmake = import('cmake')

open_cv = dependency('opencv4')

# Cmake subprojects
json_pro = subproject('json')
json = json_pro.get_variable('nlohmann_json_dep')

subdir('src')