add_executable(ApiGen
    Main.cpp)

# Targets
target_link_libraries(ApiGen SymbolIR)

if (HAS_DWARF)
    target_link_libraries(ApiGen DWARF)
endif()

if (HAS_PDB)
    target_link_libraries(ApiGen PDB)
endif()

# Other stuff
target_link_libraries(ApiGen Utility)