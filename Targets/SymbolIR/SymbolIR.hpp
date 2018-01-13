#pragma once

#include <memory>
#include <string>
#include <vector>

namespace SymbolIR {

// This format is currently WIP. It represents my initial guess for what we need, but may not 
// be representative of the final format.

struct SymbolType;
struct SymbolTypeInstance;
struct SymbolClassType;
struct SymbolEnumType;
struct SymbolPrimitiveType;
struct SymbolTemplateType;

struct SymbolFunction;
struct SymbolClassFunction;

enum SymbolProtection
{
    Public,
    Protected,
    Private,
};

struct SymbolType
{
    std::string m_Name;
    std::size_t m_Size;
};

struct SymbolTypeInstance
{
    std::shared_ptr<SymbolType> m_Type;
    bool m_IsPointer;
    bool m_IsReference;
    std::size_t m_PointerDepth;
};

struct SymbolClassType : public SymbolType
{
    struct InheritanceDescription
    {
        SymbolProtection m_Protection;
        std::shared_ptr<SymbolClassType> m_Class;
    };

    std::vector<SymbolTypeInstance> m_Data;
    std::vector<std::shared_ptr<SymbolFunction>> m_Functions;
    std::vector<InheritanceDescription> m_Parents;
};

struct SymbolEnumType : public SymbolType
{
    struct EnumDescription
    {
        std::string m_EntryName;
        std::size_t m_EntryValue;
    };

    std::vector<SymbolEnumType> m_Entries;
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

struct SymbolTemplateType : public SymbolType
{
    std::vector<SymbolTypeInstance> m_TemplateParameters;
};

struct SymbolFunction
{
    std::string m_Name;
    SymbolType m_Return;
    std::vector<SymbolType> m_Parameters;
};

struct SymbolClassFunction : public SymbolFunction
{
    SymbolProtection m_Protection;
    std::shared_ptr<SymbolClassType> m_Class;
};

struct SymbolIR
{
    std::vector<std::shared_ptr<SymbolType>> m_Types;
    std::vector<std::shared_ptr<SymbolFunction>> m_Functions;
};

}
