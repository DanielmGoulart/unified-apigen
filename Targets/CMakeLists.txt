add_subdirectory(SymbolIR)
add_subdirectory(DWARF)
add_subdirectory(PDB)

set(HAS_DWARF ${HAS_DWARF} PARENT_SCOPE)
set(HAS_PDB ${HAS_PDB} PARENT_SCOPE)