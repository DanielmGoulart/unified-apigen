#pragma once

#include "Targets/SymbolIR/SymbolIR.hpp"
#include "dwarf++.hh"

namespace DWARF::IR {

void TraverseCompilationUnit(SymbolIR::SymbolIR& ir, const dwarf::compilation_unit& unit);

}
