# ########################################################################################
# Use the code in this section for standalone projects
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(tgem)
if(NOT TARGET Garfield::Garfield)
  find_package(Garfield REQUIRED)
endif()
find_package(ROOT REQUIRED)
# ########################################################################################

# ---Build executable------------------------------------------------------------
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} source/*.cpp)

add_executable(simple_track simple_track.cpp ${SOURCES})
add_executable(tgem tgem.cpp ${SOURCES})
add_executable(view_tgem view_tgem.cpp ${SOURCES})
add_executable(gen_gasfile gen_gasfile.cpp ${SOURCES})
add_executable(plot_gasfile plot_gasfile.cpp ${SOURCES})
add_executable(tgem_gain tgem_gain.cpp ${SOURCES})

target_link_libraries(simple_track Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(tgem Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(view_tgem Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(gen_gasfile Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(plot_gasfile Garfield::Garfield ${ROOT_LIBRARIES})
target_link_libraries(tgem_gain Garfield::Garfield ${ROOT_LIBRARIES})

include_directories(${PROJECT_SOURCE_DIR}/include/ ${PROJECT_SOURCE_DIR}/include/SRIM/)

# ---Copy all files locally to the build directory-------------------------------
file(GLOB_RECURSE SRIM_DATA RELATIVE ${CMAKE_SOURCE_DIR} SRIM/*.txt)
file(GLOB_RECURSE RESOURCES RELATIVE ${CMAKE_SOURCE_DIR} resources/**)
file(GLOB_RECURSE PYSOURCE RELATIVE ${CMAKE_SOURCE_DIR} pysource/**)
file(GLOB_RECURSE SCRIPTS RELATIVE ${CMAKE_SOURCE_DIR} scripts/**)
file(GLOB_RECURSE TEMPLATES RELATIVE ${CMAKE_SOURCE_DIR} templates/**)

foreach(_file
  ${SRIM_DATA}
  ${PARAMETERS}
  ${RESOURCES}
  ${PYSOURCE}
  ${SCRIPTS}
  ${TEMPLATES})
  configure_file(${_file} ${CMAKE_CURRENT_BINARY_DIR}/${_file} COPYONLY)
endforeach()
