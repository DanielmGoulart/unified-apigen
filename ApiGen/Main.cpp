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

    FILE* test = fopen("/var/www/html/index.html", "w");
    ASSERT(test);

    for (std::size_t i = 0; i < IR.m_Symbols.size(); ++i)
    {
        std::unique_ptr<SymbolIR::Symbol>& sym = IR.m_Symbols[i];
        SymbolIR::SymbolFunction* symFunc = dynamic_cast<SymbolIR::SymbolFunction*>(sym.get());

        if (symFunc && symFunc->m_Address)
        {
            std::fprintf(test, "[%d] %s", symFunc->m_Return, symFunc->m_Name.c_str());

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

            std::fprintf(test, " = 0x%x; <br/>", symFunc->m_Address);
        }
    }
    
    fclose(test);
}
