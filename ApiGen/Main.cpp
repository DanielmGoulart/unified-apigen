#include "Targets/SymbolIR/SymbolIR.hpp"
#include "Utility/Assert.hpp"
#include "Utility/Trace.hpp"

#if HAS_DWARF
    #include "Targets/DWARF/DWARF.hpp"
#endif

int main(int argc, char** argv)
{
#if HAS_DWARF
    SymbolIR::SymbolIR IR = DWARF::GenerateIRFromExecutable("/nwnx/nwserver-local-dwarf4-nogdb");
#endif

    FILE* test = fopen("/var/www/html/api.txt", "w");
    ASSERT(test);

    for (std::size_t i = 0; i < IR.m_Symbols.size(); ++i)
    {
        std::unique_ptr<SymbolIR::Symbol>& sym = IR.m_Symbols[i];
        SymbolIR::SymbolClass* symClass = dynamic_cast<SymbolIR::SymbolClass*>(sym.get());

        if (symClass)
        {
            std::fprintf(test, "%s", symClass->m_Name.c_str());

            for (std::size_t base = 0; base < symClass->m_BaseClasses.size(); ++base)
            {
                if (base == 0)
                {
                    std::fprintf(test, " : ");
                }

                SymbolIR::SymbolIndex baseIndex = symClass->m_BaseClasses[base];
                SymbolIR::SymbolClass* symBaseClass = dynamic_cast<SymbolIR::SymbolClass*>(IR.m_Symbols[baseIndex].get());
                ASSERT(symBaseClass);

                if (symBaseClass)
                {
                    std::fprintf(test, "%s", symBaseClass->m_Name.c_str());
                }

                if (base != symClass->m_BaseClasses.size() - 1)
                {
                    std::fprintf(test, ", ");
                }
            }

            std::fprintf(test, "\n\n");

            for (std::size_t func = 0; func < symClass->m_Functions.size(); ++func)
            {
                SymbolIR::SymbolIndex funcIndex = symClass->m_Functions[func];
                SymbolIR::SymbolFunction* symFunc = dynamic_cast<SymbolIR::SymbolFunction*>(IR.m_Symbols[funcIndex].get());
                ASSERT(symFunc);

                if (symFunc)
                {
                    std::fprintf(test, "[%d] %s::%s", symFunc->m_Return, symClass->m_Name.c_str(), symFunc->m_Name.c_str());

                    if (!symFunc->m_Parameters.empty())
                    {
                        for (std::size_t param = 0; param < symFunc->m_Parameters.size(); ++param)
                        {
                            SymbolIR::SymbolFunction::NamedParameter& namedParam = symFunc->m_Parameters[param];

                            if (param == 0)
                            {
                                std::fprintf(test, "(");
                            }

                            std::fprintf(test, "[%d] %s", namedParam.m_Type, namedParam.m_Name.c_str());

                            if (param == symFunc->m_Parameters.size() - 1)
                            {
                                std::fprintf(test, ")");
                            }
                            else
                            {
                                std::fprintf(test, ", ");
                            }
                        }
                    }
                    else
                    {
                        std::fprintf(test, "()");
                    }

                    std::fprintf(test, " = 0x%x;\n", symFunc->m_Address);
                }
            }

            std::fprintf(test, "\n\n");
        }
    }
    
    fclose(test);
}
