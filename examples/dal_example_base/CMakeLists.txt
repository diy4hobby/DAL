cmake_minimum_required (VERSION 3.8)

project ("dal_example_base")

include_directories ("../../sources")
file (GLOB DAL_SRC	"../../sources/*.cpp"
					"../../sources/dal_common/*.c"
					"../../sources/dal_ser_deser/*.cpp"
					"../../sources/dal_ser_deser/dal_num_to_string/*.c"
					"../../sources/dal_ser_deser/dal_string_to_num/*.c"
					"../../sources/dal_ser_deser/json/*.c"
					"../../sources/dal_ser_deser/mpack/*.c"
					"../../sources/dal_ser_deser/webapi/*.c")

add_executable (dal_example_base ${DAL_SRC} "dal_example_base.cpp" "dal_example_base.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dal_example_base PROPERTY CXX_STANDARD 20)
endif()

