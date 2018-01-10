add_executable(ApiGen
    Main.cpp)

# Targets
target_link_libraries(ApiGen DWARF)
target_link_libraries(ApiGen PDB)

# Other stuff
target_link_libraries(ApiGen Utility)