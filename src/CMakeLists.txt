add_executable(compute-integral
  compute.cpp
  main.cpp
  routine.cpp
  utils.cpp
)

target_link_libraries(compute-integral m)
