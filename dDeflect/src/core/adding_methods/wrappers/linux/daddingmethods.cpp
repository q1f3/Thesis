#include <QProcess>
#include <QString>
#include <QDebug>

#include <core/adding_methods/wrappers/linux/daddingmethods.h>
// #include <core/adding_methods/wrappers/linux/asmcodegenerator.h>

#define mnemonic_stringify(mnemonic) \
    QString((std::string(#mnemonic).substr(std::string(#mnemonic).find_last_of(':') != std::string::npos ? \
                                  std::string(#mnemonic).find_last_of(':') + 1 : 0)).c_str()).toLower()

#define enum_stringify(enum_val) \
    QString((std::string(#enum_val).substr(std::string(#enum_val).find_last_of(':') != std::string::npos ? \
                                  std::string(#enum_val).find_last_of(':') + 1 : 0)).c_str()).toLower()

#define instruction_stringify(instruction) \
    QString((std::string(#instruction).substr(std::string(#instruction).find_last_of(':') != std::string::npos ? \
                                  std::string(#instruction).find_last_of(':') + 1 : 0)).c_str()).toLower()

const QMap<DAddingMethods::Registers_x86, QString> AsmCodeGenerator::regs_x86 = {
    { DAddingMethods::Registers_x86::EAX, enum_stringify(DAddingMethods::Registers_x86::EAX) },
    { DAddingMethods::Registers_x86::EBX, enum_stringify(DAddingMethods::Registers_x86::EBX) },
    { DAddingMethods::Registers_x86::ECX, enum_stringify(DAddingMethods::Registers_x86::ECX) },
    { DAddingMethods::Registers_x86::EDX, enum_stringify(DAddingMethods::Registers_x86::EDX) },
    { DAddingMethods::Registers_x86::EDI, enum_stringify(DAddingMethods::Registers_x86::EDI) },
    { DAddingMethods::Registers_x86::EBP, enum_stringify(DAddingMethods::Registers_x86::EBP) },
    { DAddingMethods::Registers_x86::ESI, enum_stringify(DAddingMethods::Registers_x86::ESI) },
    { DAddingMethods::Registers_x86::ESP, enum_stringify(DAddingMethods::Registers_x86::ESP) }
};

const QMap<DAddingMethods::Registers_x64, QString> AsmCodeGenerator::regs_x64 = {
    { DAddingMethods::Registers_x64::RAX, enum_stringify(DAddingMethods::Registers_x64::RAX) },
    { DAddingMethods::Registers_x64::RBX, enum_stringify(DAddingMethods::Registers_x64::RBX) },
    { DAddingMethods::Registers_x64::RCX, enum_stringify(DAddingMethods::Registers_x64::RCX) },
    { DAddingMethods::Registers_x64::RDX, enum_stringify(DAddingMethods::Registers_x64::RDX) },
    { DAddingMethods::Registers_x64::RDI, enum_stringify(DAddingMethods::Registers_x64::RDI) },
    { DAddingMethods::Registers_x64::RSI, enum_stringify(DAddingMethods::Registers_x64::RSI) },
    { DAddingMethods::Registers_x64::RBP, enum_stringify(DAddingMethods::Registers_x64::RBP) },
    { DAddingMethods::Registers_x64::RSP, enum_stringify(DAddingMethods::Registers_x64::RSP) },
    { DAddingMethods::Registers_x64::R8 , enum_stringify(DAddingMethods::Registers_x64::R8)  },
    { DAddingMethods::Registers_x64::R9 , enum_stringify(DAddingMethods::Registers_x64::R9)  },
    { DAddingMethods::Registers_x64::R10, enum_stringify(DAddingMethods::Registers_x64::R10) },
    { DAddingMethods::Registers_x64::R11, enum_stringify(DAddingMethods::Registers_x64::R11) },
    { DAddingMethods::Registers_x64::R12, enum_stringify(DAddingMethods::Registers_x64::R12) },
    { DAddingMethods::Registers_x64::R13, enum_stringify(DAddingMethods::Registers_x64::R13) },
    { DAddingMethods::Registers_x64::R14, enum_stringify(DAddingMethods::Registers_x64::R14) },
    { DAddingMethods::Registers_x64::R15, enum_stringify(DAddingMethods::Registers_x64::R15) }
};

const QMap<AsmCodeGenerator::Instructions, QString> AsmCodeGenerator::instructions = {
    { AsmCodeGenerator::Instructions::POP , instruction_stringify(AsmCodeGenerator::Instructions::POP)  },
    { AsmCodeGenerator::Instructions::PUSH, instruction_stringify(AsmCodeGenerator::Instructions::PUSH) },
    { AsmCodeGenerator::Instructions::MOV,  instruction_stringify(AsmCodeGenerator::Instructions::MOV)  },
    { AsmCodeGenerator::Instructions::JMP,  instruction_stringify(AsmCodeGenerator::Instructions::JMP)  }
};

DAddingMethods::DAddingMethods() {
    placeholder_id = {
        { PlaceholderTypes::PARAM_PRE,          QString("(?^_^")     },
        { PlaceholderTypes::PARAM_POST,         QString("^_^?)")     },
        { PlaceholderTypes::PLACEHOLDER_PRE,    QString("(rsj?^_^")  },
        { PlaceholderTypes::PLACEHOLDER_POST,   QString("^_^?rsj)")  }
    };

    placeholder_mnm = {
        { PlaceholderMnemonics::DDETECTIONHANDLER,  mnemonic_stringify(PlaceholderMnemonics::DDETECTIONHANDLER) },
        { PlaceholderMnemonics::DDETECTIONMETHOD,   mnemonic_stringify(PlaceholderMnemonics::DDETECTIONMETHOD)  },
        { PlaceholderMnemonics::DDRET,              mnemonic_stringify(PlaceholderMnemonics::DDRET)             }
    };
}

uint64_t DAddingMethods::fill_params(QString &code, const QMap<QString, QString> &params) {
    uint64_t cnt = 0;
    foreach (QString param, params.keys()) {
        QString plc_param(QString("%1%2%3").arg(placeholder_id[PlaceholderTypes::PARAM_PRE], param,
                                                placeholder_id[PlaceholderTypes::PARAM_POST]));

        // look in string for plc_param cahnge it to value from dictionary
        code.replace(plc_param, params[param]);
    }
    return cnt;
}

uint64_t DAddingMethods::fill_placeholders(QString &code, const QString &gen_code, PlaceholderMnemonics plc_mnm) {
    uint64_t cnt = 0;

    code.replace(QString("%1%2%3").arg(placeholder_id[PlaceholderTypes::PLACEHOLDER_PRE], placeholder_mnm[plc_mnm],
                                       placeholder_id[PlaceholderTypes::PLACEHOLDER_POST]), gen_code);

    return cnt;
}

bool DAddingMethods::compile(const QString &code2compile, QByteArray &compiled_code) {
    // TODO: change
    QFile file("tocompile.asm");
    if (!file.open(QIODevice::WriteOnly))
        return false;

    if (file.write(code2compile.toStdString().c_str(), code2compile.length()) != code2compile.length())
        return false;

    file.close();

    if (QProcess::execute("nasm", { "tocompile.asm" }))
        return false;

    file.setFileName("tocompile");

    if (!file.open(QIODevice::ReadOnly))
        return false;

    compiled_code = file.readAll();

    file.close();
    return true;
}
