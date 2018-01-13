if (NOT OS_LINUX)
    set(HAS_DWARF 0 PARENT_SCOPE)
    return()
endif()

set(HAS_DWARF 1 PARENT_SCOPE)

add_library(DWARF STATIC
    DWARF.cpp DWARF.hpp)

target_link_libraries(DWARF Utility)
target_link_libraries(DWARF SymbolIR)

# Link libelfin.
add_dependencies(DWARF libelfin)
target_link_libraries(DWARF ${LIBELF_STATIC_PATH} ${LIBDWARF_STATIC_PATH})
target_include_directories(DWARF PRIVATE ${LIBELF_INCLUDE_PATH} ${LIBDWARF_INCLUDE_PATH})