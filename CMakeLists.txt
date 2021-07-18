cmake_minimum_required(VERSION 3.10)
project(otus6 VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS ON)

add_compile_options(-Wall -Wextra -pedantic -Werror)

find_package(Boost COMPONENTS unit_test_framework program_options filesystem REQUIRED)

add_library(bulk_lib bulk.cpp bulk.h response_handler.cpp response_handler.h)

add_executable(otus6 main.cpp)
set_target_properties(otus6 PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
        )
target_link_libraries(otus6 ${Boost_LIBRARIES} bulk_lib)

add_executable(test_bulk test_bulk.cpp)
set_target_properties(test_bulk PROPERTIES
        INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)
target_link_libraries(test_bulk bulk_lib ${Boost_LIBRARIES})

enable_testing()
add_test(test_bulk test_bulk)


install(TARGETS otus6 RUNTIME DESTINATION bin)
set(CPACK_GENERATOR DEB)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEB_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${otus6_VERSION})
set(CPACK_PACKAGE_CONTACT buevichd@yandex-team.ru)
include(CPack)

