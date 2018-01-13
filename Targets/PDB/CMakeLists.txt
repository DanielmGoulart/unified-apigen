# TODO - PDB
message("No PDB support yet. Skipping.")
set(HAS_PDB 0 PARENT_SCOPE)
return()

set(HAS_PDB 1 PARENT_SCOPE)

add_library(PDB STATIC
    PDB.cpp PDB.hpp)

target_link_libraries(PDB Utility)
target_link_libraries(PDB SymbolIR)