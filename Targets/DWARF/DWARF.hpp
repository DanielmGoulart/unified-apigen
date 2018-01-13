#pragma once

#include "Targets/SymbolIR/SymbolIR.hpp"

namespace DWARF {

SymbolIR::SymbolIR GenerateIRFromExecutable(const std::string& path);

}
