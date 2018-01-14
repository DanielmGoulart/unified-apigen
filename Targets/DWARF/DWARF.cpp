#include "Targets/DWARF/DWARF.hpp"
#include "Targets/DWARF/DWARFIR.hpp"
#include "Utility/Assert.hpp"

#include "elf++.hh"
#include "dwarf++.hh"
#include <cstdio>

namespace DWARF {

SymbolIR::SymbolIR GenerateIRFromExecutable(const std::string& path)
{
    FILE* binary = std::fopen(path.c_str(), "r");
    ASSERT(binary);

    if (!binary)
    {
        return SymbolIR::SymbolIR();
    }

    elf::elf elfyelf(elf::create_mmap_loader(fileno(binary)));

    dwarf::dwarf dwarfydwarf(dwarf::elf::create_loader(elfyelf));

    SymbolIR::SymbolIR ir;

    for (const dwarf::compilation_unit& unit : dwarfydwarf.compilation_units())
    {
        IR::TraverseCompilationUnit(ir, unit);
    }

    return ir;
}

}