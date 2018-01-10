#include <memory>
#include <string>
#include <vector>

// WIP

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

struct SymbolEnumType : public SymbolType
{
    struct EnumDescription
    {
        std::string m_EntryName;
        std::size_t m_EntryValue;
    };

    std::vector<SymbolEnumType> m_Entries;
}

enum SymbolProtection
{
    Public,
    Protected,
    Private,
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
