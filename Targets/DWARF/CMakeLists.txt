add_library(DWARF STATIC
    DWARF.cpp DWARF.hpp)

target_link_libraries(DWARF Utility)
target_link_libraries(DWARF SymbolIR)
target_link_libraries(DWARF libdwarf)