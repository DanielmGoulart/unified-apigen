#pragma once

#include <memory>
#include <string>
#include <vector>

namespace SymbolIR {

// This format is currently WIP. It represents my initial guess for what we need, but may not 
// be representative of the final format.

struct Symbol;
struct SymbolType;
struct SymbolPrimitiveType;

struct SymbolStructure;
struct SymbolClass;
struct SymbolEnum;

struct SymbolFunction;
struct SymbolClassFunction;

// NOTE: 0 is a magic number here. It means there is nothing there.
// All of our indices start at 1.
using SymbolIndex = std::size_t;

struct Symbol
{
    // Empty base class used for up/downcasting between symbols.
    virtual ~Symbol() = default;
};

struct SymbolType : public Symbol
{
    std::string m_Name;
    std::size_t m_BitSize;
};

struct SymbolPrimitiveType : public SymbolType
{
    enum Type
    {
        U8,
        I8,
        U16,
        I16,
        U32,
        I32,
        U64,
        I64,
        Float,
        Double,
        Void
    };

    Type m_PrimitiveType;
};

struct SymbolStructure : public SymbolType
{
    // Empty base class for hierarchy.
};

struct SymbolClass : public SymbolStructure
{
    std::vector<SymbolIndex> m_Members;
    std::vector<SymbolIndex> m_Functions;
    std::vector<SymbolIndex> m_Structures;
    std::vector<SymbolIndex> m_BaseClasses;
};

struct SymbolEnum : public SymbolStructure
{
    struct EnumDescription
    {
        std::string m_EntryName;
        std::size_t m_EntryValue;
    };

    std::vector<SymbolEnum> m_Entries;
};

struct SymbolFunction : public Symbol
{
    struct NamedParameter
    {
        std::string m_Name;
        SymbolIndex m_Type;
    };

    std::string m_Name;
    SymbolIndex m_Return;
    std::vector<NamedParameter> m_Parameters;
    std::uintptr_t m_Address;
};

struct SymbolIR
{
    std::vector<std::unique_ptr<Symbol>> m_Symbols;
};

}
