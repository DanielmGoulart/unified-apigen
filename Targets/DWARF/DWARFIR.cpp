#include "Targets/DWARF/DWARFIR.hpp"
#include "Utility/Assert.hpp"
#include "Utility/Trace.hpp"
#include <unordered_map>

namespace DWARF::IR {

namespace {

// GCC extensions to the DWARF format.
static constexpr dwarf::DW_TAG DW_TAG_GCC_1 = static_cast<dwarf::DW_TAG>(0x4109);

static constexpr dwarf::DW_AT DW_AT_GCC_1 = static_cast<dwarf::DW_AT>(0x87);
static constexpr dwarf::DW_AT DW_AT_GCC_2 = static_cast<dwarf::DW_AT>(0x2116);
static constexpr dwarf::DW_AT DW_AT_GCC_3 = static_cast<dwarf::DW_AT>(0x2117);

void DEBUG_RecursePrint(const dwarf::die& die, int depth = 0)
{
    TRACE("[%d] <%llx> %s", depth, die.get_section_offset(), to_string(die.tag).c_str());

    for (auto& attr : die.attributes())
    {
        TRACE("  %s %s\n", to_string(attr.first).c_str(), to_string(attr.second).c_str());
    }

    for (auto& node : die)
    {
        DEBUG_RecursePrint(node, depth + 1);
    }
}

std::unordered_map<dwarf::section_offset, SymbolIR::SymbolIndex> s_OffsetToSymbolIndexMap;
std::unordered_map<SymbolIR::SymbolIndex, dwarf::section_offset> s_SymbolIndexToOffsetMap;

bool GetIRSymbolIndexFromDIE(SymbolIR::SymbolIR& ir, dwarf::section_offset offset, SymbolIR::SymbolIndex* out)
{
    ASSERT(out);

    auto iter = s_OffsetToSymbolIndexMap.find(offset);

    if (iter != std::end(s_OffsetToSymbolIndexMap))
    {
        *out = iter->second;
        return true;
    }

    static SymbolIR::SymbolIndex s_NextTypeIndex = 1;
    SymbolIR::SymbolIndex index = s_NextTypeIndex++;
    s_OffsetToSymbolIndexMap.insert(std::make_pair(offset, index));
    s_SymbolIndexToOffsetMap.insert(std::make_pair(index, offset));

    if (ir.m_Symbols.size() <= s_NextTypeIndex)
    {
        ir.m_Symbols.resize(s_NextTypeIndex + 1);
    }

    *out = index;
    return true;
}

dwarf::section_offset GetDIEFromSymbolIndex(SymbolIR::SymbolIR& ir, SymbolIR::SymbolIndex index)
{
    auto iter = s_SymbolIndexToOffsetMap.find(index);
    ASSERT(iter != std::end(s_SymbolIndexToOffsetMap));
    return iter->first;
}

}

SymbolIR::SymbolIndex BuildTypeFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent);
SymbolIR::SymbolIndex BuildStructureFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent);
SymbolIR::SymbolIndex BuildFunctionFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent);

SymbolIR::SymbolIndex BuildTypeFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent)
{
    return SymbolIR::SymbolIndex();
}

void ParseStructureAttributes(SymbolIR::SymbolIR& ir, SymbolIR::SymbolClass* symbolClass, const dwarf::die& die, bool first = false)
{
    for (auto& attributePair : die.attributes())
    {
        dwarf::DW_AT attribute = attributePair.first;
        dwarf::value value = attributePair.second;

        if (attribute == dwarf::DW_AT::declaration)
        {
            if (first)
            {
                symbolClass->m_Declaration = value.as_flag();
            }
        }
        else if (attribute == dwarf::DW_AT::name)
        {
            symbolClass->m_Name = value.as_string();
        }
        else
        {
            TRACE("Unhandled attribute %s %s at structure level.",
                to_string(attribute).c_str(),
                to_string(value).c_str());
        }
    }
}

void ParseStructureChildren(SymbolIR::SymbolIR& ir, SymbolIR::SymbolClass* symbolClass, const dwarf::die& die, bool first = false)
{
    for (const dwarf::die& child : die)
    {
        if (child.tag == dwarf::DW_TAG::subprogram) // function
        {
            SymbolIR::SymbolIndex function = BuildFunctionFromDIE(ir, child, die);
            if (function)
            {
                symbolClass->m_Functions.push_back(function);
            }
        }
        else if (child.tag == dwarf::DW_TAG::member)
        {
            // TODO: How to handle?
        }
        else if (child.tag == dwarf::DW_TAG::class_type ||
            child.tag == dwarf::DW_TAG::structure_type ||
            child.tag == dwarf::DW_TAG::enumeration_type ||
            child.tag == dwarf::DW_TAG::union_type)
        {
            SymbolIR::SymbolIndex nestedStructure = BuildStructureFromDIE(ir, child, die);
            if (nestedStructure)
            {
                symbolClass->m_Structures.push_back(nestedStructure);
            }
        }
        else if (child.tag == DW_TAG_GCC_1)
        {
            // Intentionally ignored.
        }
        else
        {
            TRACE("Unhandled die %s at structure level.", to_string(child.tag).c_str());
            DEBUG_RecursePrint(child);
        }
    }
}

SymbolIR::SymbolIndex BuildStructureFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent)
{
    if (die.tag == dwarf::DW_TAG::class_type || die.tag == dwarf::DW_TAG::structure_type)
    {
        SymbolIR::SymbolIndex structureIndex;
        dwarf::section_offset offset = die.get_section_offset();
        GetIRSymbolIndexFromDIE(ir, offset, &structureIndex);

        ir.m_Symbols[structureIndex] = std::make_unique<SymbolIR::SymbolClass>();
        SymbolIR::SymbolClass* symbolClass = dynamic_cast<SymbolIR::SymbolClass*>(ir.m_Symbols[structureIndex].get());

        ParseStructureAttributes(ir, symbolClass, die, true);
        ParseStructureChildren(ir, symbolClass, die, true);

        return structureIndex;
    }
    else if (die.tag == dwarf::DW_TAG::enumeration_type)
    {
        // TODO: How to handle?
    }
    else if (die.tag == dwarf::DW_TAG::union_type)
    {
        // TODO: How to handle?
    }
    else
    {
        DEBUG_RecursePrint(die);
        ASSERT_FAIL();
    }

    return SymbolIR::SymbolIndex();
}

void ParseFunctionAttributes(SymbolIR::SymbolIR& ir, SymbolIR::SymbolFunction* symbolFunction, const dwarf::die& die, bool first = false)
{
    for (auto& attributePair : die.attributes())
    {
        dwarf::DW_AT attribute = attributePair.first;
        dwarf::value value = attributePair.second;

        if (attribute == dwarf::DW_AT::declaration)
        {
            if (first)
            {
                symbolFunction->m_Declaration = value.as_flag();
            }
        }
        else if (attribute == dwarf::DW_AT::name)
        {
            symbolFunction->m_Name = value.as_string();
        }
        else if (attribute == dwarf::DW_AT::type)
        {
            dwarf::die returnDie = value.as_reference();
            GetIRSymbolIndexFromDIE(ir, returnDie.get_section_offset(), &symbolFunction->m_Return);
        }
        else if (attribute == dwarf::DW_AT::low_pc) // address
        {
            symbolFunction->m_Address = value.as_address();
        }
        else if (attribute == dwarf::DW_AT::specification) // reference to another DIE
        {
            dwarf::die child = value.as_reference();
            ParseFunctionAttributes(ir, symbolFunction, child);
        }
        else if (attribute == dwarf::DW_AT::abstract_origin)
        {
            dwarf::die child = value.as_reference();
            ParseFunctionAttributes(ir, symbolFunction, child);
        }
        else if (attribute == dwarf::DW_AT::artificial) // compiler generated (like thisptr)
        {
            if (first)
            {
                symbolFunction->m_Artificial = value.as_flag();
            }

            // TODO: Do we need to handle this here?
        }
        else if (attribute == dwarf::DW_AT::external || // defined in another compilation unit
            attribute == dwarf::DW_AT::decl_file ||
            attribute == dwarf::DW_AT::decl_line ||
            attribute == dwarf::DW_AT::declaration || // ??
            attribute == dwarf::DW_AT::sibling || // ??
            attribute == dwarf::DW_AT::linkage_name || // mangled name
            attribute == dwarf::DW_AT::object_pointer || // thisptr, don't think we need
            attribute == dwarf::DW_AT::inline_ ||
            attribute == dwarf::DW_AT::frame_base ||
            attribute == dwarf::DW_AT::location || // ??, probably the section or compilation unit
            attribute == dwarf::DW_AT::high_pc || // ??, something to do with addresses
            attribute == dwarf::DW_AT::accessibility || // public / etc
            attribute == DW_AT_GCC_1 ||
            attribute == DW_AT_GCC_2 ||
            attribute == DW_AT_GCC_3)
        {
            // Intentionally ignored.
        }
        else
        {
            TRACE("Unhandled attribute %s %s at function level.",
                to_string(attribute).c_str(),
                to_string(value).c_str());
        }
    }
}

void ParseFunctionChildren(SymbolIR::SymbolIR& ir, SymbolIR::SymbolFunction* symbolFunction, const dwarf::die& die, bool first = false)
{
    for (const dwarf::die& child : die)
    {
        if (child.tag == dwarf::DW_TAG::formal_parameter)
        {
            bool artificial = false;
            SymbolIR::SymbolFunction::NamedParameter parameter;

            for (auto& attributePair : child.attributes())
            {
                dwarf::DW_AT attribute = attributePair.first;
                dwarf::value value = attributePair.second;

                if (attribute == dwarf::DW_AT::name) // obvious
                {
                    parameter.m_Name = value.as_string();
                }
                else if (attribute == dwarf::DW_AT::type) // obvious
                {
                    dwarf::die parameterDie = value.as_reference();
                    SymbolIR::SymbolIndex parameterIndex;
                    GetIRSymbolIndexFromDIE(ir, parameterDie.get_section_offset(), &parameterIndex);
                    parameter.m_Type = parameterIndex;
                }
                else if (attribute == dwarf::DW_AT::artificial) // compiler generated (like thisptr)
                {
                    // TODO: Is this really how we handle this? Maybe.
                    artificial = value.as_flag();
                    symbolFunction->m_Artificial = artificial;
                }
                else if (attribute == dwarf::DW_AT::decl_file ||
                    attribute == dwarf::DW_AT::decl_line ||
                    attribute == dwarf::DW_AT::abstract_origin || // something to do with inline
                    attribute == dwarf::DW_AT::location) // ??, probably the section or compilation unit
                {
                    // Intentionally ignored.
                }
                else
                {
                    TRACE("Unhandled attribute %s %s at function formal parameter level.",
                        to_string(attribute).c_str(),
                        to_string(value).c_str());
                }
            }

            if (!artificial)
            {
                symbolFunction->m_Parameters.push_back(parameter);
            }
        }
        else if (child.tag == dwarf::DW_TAG::variable) // variables on the stack - it would be cool to print these
        {
            // TODO: How to handle?
        }
        else if (child.tag == dwarf::DW_TAG::inlined_subroutine || // Something has been inlined
            child.tag == dwarf::DW_TAG::unspecified_parameters || // ??
            child.tag == DW_TAG_GCC_1)
        {
            // Intentionally ignored.
        }
        else
        {
            TRACE("Unhandled die %s at function level.", to_string(child.tag).c_str());
            DEBUG_RecursePrint(child);
        }
    }
}

SymbolIR::SymbolIndex BuildFunctionFromDIE(SymbolIR::SymbolIR& ir, const dwarf::die& die, const dwarf::die& parent)
{
    if (die.tag == dwarf::DW_TAG::subprogram)
    {
        SymbolIR::SymbolIndex functionIndex;
        dwarf::section_offset offset = die.get_section_offset();
        GetIRSymbolIndexFromDIE(ir, offset, &functionIndex);

        ir.m_Symbols[functionIndex] = std::make_unique<SymbolIR::SymbolFunction>();
        SymbolIR::SymbolFunction* symbolFunction = dynamic_cast<SymbolIR::SymbolFunction*>(ir.m_Symbols[functionIndex].get());

        ParseFunctionAttributes(ir, symbolFunction, die, true);
        ParseFunctionChildren(ir, symbolFunction, die, true);

        return functionIndex;
    }
    else
    {
        DEBUG_RecursePrint(die);
        ASSERT_FAIL();
    }

    return SymbolIR::SymbolIndex();
}

void TraverseRootDIE(SymbolIR::SymbolIR& ir, const dwarf::die& root)
{
    for (const dwarf::die& child : root)
    {
        if (child.tag == dwarf::DW_TAG::array_type ||
            child.tag == dwarf::DW_TAG::base_type ||
            child.tag == dwarf::DW_TAG::const_type ||
            child.tag == dwarf::DW_TAG::pointer_type ||
            child.tag == dwarf::DW_TAG::reference_type ||
            child.tag == dwarf::DW_TAG::subroutine_type) // funcptr
        {
            BuildTypeFromDIE(ir, child, root);
        }
        else if (child.tag == dwarf::DW_TAG::class_type ||
            child.tag == dwarf::DW_TAG::enumeration_type ||
            child.tag == dwarf::DW_TAG::structure_type ||
            child.tag == dwarf::DW_TAG::union_type)
        {
            BuildStructureFromDIE(ir, child, root);
        }
        else if (child.tag == dwarf::DW_TAG::subprogram)
        {
            BuildFunctionFromDIE(ir, child, root);
        }
        else if (child.tag == dwarf::DW_TAG::typedef_)
        {
            // TODO: How to handle?
        }
        else if (child.tag == dwarf::DW_TAG::namespace_)
        {
            TraverseRootDIE(ir, child);
        }
        else if (child.tag == dwarf::DW_TAG::variable) // this is super cool, we can expose globals
        {
            // TODO: How to handle?
        }
        else if (child.tag == dwarf::DW_TAG::imported_declaration) // probably not needed
        {
            // Intentionally ignored.
        }
        else
        {
            TRACE("Unhandled die %s at compilation unit level.", to_string(child.tag).c_str());
            DEBUG_RecursePrint(child);
        }
    }
}

void TraverseCompilationUnit(SymbolIR::SymbolIR& ir, const dwarf::compilation_unit& unit)
{
    TraverseRootDIE(ir, unit.root());
}

}
