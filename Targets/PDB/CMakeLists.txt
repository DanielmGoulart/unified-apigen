add_library(PDB STATIC
    PDB.cpp PDB.hpp)

target_link_libraries(PDB Utility)
target_link_libraries(PDB SymbolIR)