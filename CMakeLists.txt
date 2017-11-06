cmake_minimum_required(VERSION 3.1)
project(score_addon_berlioz LANGUAGES CXX)

add_subdirectory("${OSSIA_3RDPARTY_FOLDER}/pybind11" pybind11)
# General initialization
score_common_setup()

# Source files
set(HDRS
    berlioz/Executor.hpp
    berlioz/Inspector.hpp
    berlioz/LocalTree.hpp
    berlioz/Metadata.hpp
    berlioz/Presenter.hpp
    berlioz/Process.hpp
    berlioz/View.hpp
    berlioz/Layer.hpp
    berlioz/CommandFactory.hpp

    score_addon_berlioz.hpp
)
set(SRCS
    berlioz/Executor.cpp
    berlioz/Inspector.cpp
    berlioz/LocalTree.cpp
    berlioz/Presenter.cpp
    berlioz/Process.cpp
    berlioz/View.cpp

    score_addon_berlioz.cpp
)


# Creation of the library
add_library(${PROJECT_NAME} ${SRCS} ${HDRS})

# Code generation
score_generate_command_list_file(${PROJECT_NAME} "${HDRS}")

# Link
target_link_libraries(${PROJECT_NAME} PUBLIC score_plugin_engine pybind11::embed)

# Target-specific options
setup_score_plugin(${PROJECT_NAME})
