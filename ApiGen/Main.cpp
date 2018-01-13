#include "Targets/SymbolIR/SymbolIR.hpp"

#if HAS_DWARF
    #include "Targets/DWARF/DWARF.hpp"
#endif

int main(int argc, char** argv)
{
#if HAS_DWARF
    SymbolIR::SymbolIR IR = DWARF::GenerateIRFromExecutable("/nwnx/nwserver");
#endif
}
