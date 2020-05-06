add_executable(example main.c)
target_link_libraries(example myvk)
target_include_directories(example PUBLIC ..)