#include "test_elf.h"

#include <QFile>
#include <QDir>
#include <QVariant>
#include <QDebug>
#include <core/file_types/elffile.h>
#include <core/adding_methods/wrappers/linux/daddingmethods.h>

int oep_ptrace(const QString &elf_fname, const QString &ptrace_fname, const QString &elf_out) {
    ELF elf(elf_fname);
    qDebug() << "valid: " << elf.is_valid();
    qDebug() << "segments no: " << elf.get_number_of_segments();
    QFile first_test(ptrace_fname);
    if (!first_test.open(QIODevice::ReadOnly))
        return 1;
    QByteArray whole = first_test.readAll();
    // TODO: should be in wrapper after add jump instruction here
    Elf64_Addr oldep;
    if (!elf.get_entry_point(oldep))
        return 1;
    QByteArray jmp;
    if (elf.is_x86() || oldep < 0x100000000) {
        jmp.append(0xb8); // mov eax, addr
        // oldep -= 0x5;
        for(uint i = 0; i < sizeof(Elf32_Addr); ++i) {
            int a = (oldep >> (i * 8) & 0xff);
            jmp.append(static_cast<char>(a));
        }
        jmp.append("\xff\xe0", 2); // jmp eax
    }
    else if (elf.is_x64()) {
        jmp.append("\x48\xb8", 2); // mov rax, addr
        for(uint i = 0; i < sizeof(Elf64_Addr); ++i) {
            int a = (oldep >> (i * 8) & 0xff);
            jmp.append(static_cast<char>(a));
        }
        jmp.append("\xff\xe0", 2);
    }
    else return 1;
    // QByteArray nops(12, '\x90');
    Elf64_Addr nva;
    whole.append(jmp);
    QByteArray nf = elf.extend_segment(whole, true, nva);
    if (!elf.set_entry_point(nva, nf))
        return 1;
    qDebug() << "new entry point: " << QString("0x%1").arg(nva, 0, 16);
    elf.write_to_file(elf_out, nf);
    qDebug() << "saving to file: " << elf_out;
    return 0;
}

int create_thread(const QString &elf_fname, const QString &thread_fname, const QString &elf_out) {
    ELF elf(elf_fname);
    qDebug() << "valid: " << elf.is_valid();
    qDebug() << "segments no: " << elf.get_number_of_segments();
    QFile first_test(thread_fname);
    if (!first_test.open(QIODevice::ReadOnly))
        return 1;
    QByteArray whole = first_test.readAll();
    // TODO: should be in wrapper after add jump instruction here
    Elf64_Addr oldep;
    if (!elf.get_entry_point(oldep))
        return 1;
    QByteArray jmp;
    if (elf.is_x86() || oldep < 0x100000000) {
        jmp.append(0xb8); // mov eax, addr
        // oldep -= 0x5;
        for(uint i = 0; i < sizeof(Elf32_Addr); ++i) {
            int a = (oldep >> (i * 8) & 0xff);
            jmp.append(static_cast<char>(a));
        }
        jmp.append("\xff\xe0", 2); // jmp eax
    }
    else if (elf.is_x64()) {
        jmp.append("\x48\xb8", 2); // mov rax, addr
        for(uint i = 0; i < sizeof(Elf64_Addr); ++i) {
            int a = (oldep >> (i * 8) & 0xff);
            jmp.append(static_cast<char>(a));
        }
        jmp.append("\xff\xe0", 2);
    }
    else return 1;
    // QByteArray nops(12, '\x90');
    Elf64_Addr nva;
    whole.append(jmp);
    QByteArray nf = elf.extend_segment(whole, true, nva);
    if (!elf.set_entry_point(nva, nf))
        return 1;
    qDebug() << "new entry point: " << QString("0x%1").arg(nva, 0, 16);
    elf.write_to_file(elf_out, nf);
    qDebug() << "saving to file: " << elf_out;
    return 0;
}

void test() {
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for my 32-bit app...";
    // test oep + ptrace
    if (oep_ptrace("my32", "ptrace", "_my32")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for my 64-bit app...";
    if (oep_ptrace("my64", "ptrace64", "_my64")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for derby 32-bit app...";
    // test oep + ptrace
    if (oep_ptrace("derby32", "ptrace", "_derby32")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for derby 64-bit app...";
    if (oep_ptrace("derby64", "ptrace64", "_derby64")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for edb 64-bit app...";
    if (oep_ptrace("edb", "ptrace64", "_edb64")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for dDeflect 64-bit app...";
    if (oep_ptrace("dDeflect", "ptrace64", "_dDeflect64")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for dDeflect 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for my 32-bit app...";
    // test oep + ptrace
    if (oep_ptrace("my32", "thread", "_my32t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for my 64-bit app...";
    if (oep_ptrace("my64", "thread64", "_my64t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for derby 32-bit app...";
    // test oep + ptrace
    if (oep_ptrace("derby32", "thread", "_derby32t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for derby 64-bit app...";
    if (oep_ptrace("derby64", "thread64", "_derby64t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for edb 64-bit app...";
    if (oep_ptrace("edb", "thread64", "_edb64t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for dDeflect 64-bit app...";
    if (oep_ptrace("dDeflect", "thread64", "_dDeflect64t")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for dDeflect 64-bit app done";
}

int test_flagx(const QString &elf_fname, const QString &elf_out) {
    ELF elf(elf_fname);
    qDebug() << "valid: " << elf.is_valid();
    qDebug() << "segments no: " << elf.get_number_of_segments();
    // QByteArray nops(12, '\x90');
    QByteArray nops(5, '\x00');
    // TODO: should be in wrapper after add jump instruction here
    Elf64_Addr oldep;
    if (!elf.get_entry_point(oldep))
        return 1;
    Elf64_Addr nva;
    QByteArray nf = elf.extend_segment(nops, true, nva);
    /*if (!elf.set_entry_point(nva, nf))
return 1;*/
    qDebug() << "new entry point: " << QString("0x%1").arg(nva, 0, 16);
    elf.write_to_file(elf_out, nf);
    qDebug() << "saving to file: " << elf_out;
    return 0;
}

bool test_oep_wrappers(const QString &elf_fname, const QString &wrapper,
                       const QString &method, const QString &handl) {
    // DAddingMethods::InjectDescription inject_desc;
    ELF elf(elf_fname);
    // qDebug() << "valid: " << elf.is_valid();
    if (!elf.is_valid())
        return false;
    QFile code;
    DAddingMethods dam;
    if (elf.is_x86()) {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x86> inject_desc;
        DAddingMethods::OEPWrapper<DAddingMethods::Registers_x86> oepwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> oepaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "127" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        oepaction.code = code.readAll();
        oepaction.detect_handler = nullptr;
        oepaction.ret = DAddingMethods::Registers_x86::EAX;
        oepaction.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                                DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                                DAddingMethods::Registers_x86::ESI };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        oepwrapper.detect_handler = &handler;
        oepwrapper.oep_action = &oepaction;
        oepwrapper.code = code.readAll();
        oepwrapper.used_regs = { DAddingMethods::Registers_x86::EDI };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        inject_desc.adding_method = &oepwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x86>(elf, inject_desc))
            return false;
    }
    else {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x64> inject_desc;
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        DAddingMethods::OEPWrapper<DAddingMethods::Registers_x64> oepwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> oepaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "188" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        oepaction.code = code.readAll();
        oepaction.detect_handler = nullptr;
        oepaction.ret = DAddingMethods::Registers_x64::RAX;
        oepaction.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                                DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                                DAddingMethods::Registers_x64::R10 };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        oepwrapper.detect_handler = &handler;
        oepwrapper.oep_action = &oepaction;
        oepwrapper.code = code.readAll();
        oepwrapper.used_regs = { DAddingMethods::Registers_x64::R11, DAddingMethods::Registers_x64::R12 };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        inject_desc.adding_method = &oepwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x64>(elf, inject_desc))
            return false;
    }
    // rename file on hard drive
    QFile::rename("template", QString("_%1o").arg(elf_fname));
    qDebug() << "saving to file: " << QString("_%1o").arg(elf_fname);
    return true;
}

bool test_thread_wrappers(const QString &elf_fname, const QString &wrapper,
                          const QString &method, const QString &handl) {
    ELF elf(elf_fname);
    // qDebug() << "valid: " << elf.is_valid();
    if (!elf.is_valid())
        return false;
    QFile code;
    DAddingMethods dam;
    if (elf.is_x86()) {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x86> inject_desc;
        DAddingMethods::ThreadWrapper<DAddingMethods::Registers_x86> twrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> taction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "127" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        taction.code = code.readAll();
        taction.detect_handler = nullptr;
        taction.ret = DAddingMethods::Registers_x86::EAX;
        taction.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                              DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                              DAddingMethods::Registers_x86::ESI, DAddingMethods::Registers_x86::EDI };
        // qDebug() << taction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        twrapper.detect_handler = &handler;
        twrapper.thread_action = &taction;
        twrapper.code = code.readAll();
        twrapper.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                               DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                               DAddingMethods::Registers_x86::ESI, DAddingMethods::Registers_x86::EDI };
        twrapper.params = { { "sleep1", "0" },
                            { "sleep2", "5" } };
        // qDebug() << twrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::Thread;
        inject_desc.adding_method = &twrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x86>(elf, inject_desc))
            return false;
    }
    else {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x64> inject_desc;
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        DAddingMethods::ThreadWrapper<DAddingMethods::Registers_x64> twrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> taction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "188" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        taction.code = code.readAll();
        taction.detect_handler = nullptr;
        taction.ret = DAddingMethods::Registers_x64::RAX;
        taction.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                              DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                              DAddingMethods::Registers_x64::R10 };
        // qDebug() << taction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        twrapper.detect_handler = &handler;
        twrapper.thread_action = &taction;
        twrapper.code = code.readAll();
        twrapper.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                               DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                               DAddingMethods::Registers_x64::R10, DAddingMethods::Registers_x64::R8 };
        twrapper.params = { { "sleep1", "0" },
                            { "sleep2", "5" } };
        // qDebug() << twrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::Thread;
        inject_desc.adding_method = &twrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x64>(elf, inject_desc))
            return false;
    }
    // rename file on hard drive
    QFile::rename("template", QString("_%1t").arg(elf_fname));
    qDebug() << "saving to file: " << QString("_%1t").arg(elf_fname);
    return true;
}

bool test_init_oep_wrappers(const QString &elf_fname, const QString &wrapper,
                            const QString &method, const QString &handl) {
    // DAddingMethods::InjectDescription inject_desc;
    ELF elf(elf_fname);
    // qDebug() << "valid: " << elf.is_valid();
    if (!elf.is_valid())
        return false;
    QFile code;
    DAddingMethods dam;
    if (elf.is_x86()) {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x86> inject_desc;
        DAddingMethods::TrampolineWrapper<DAddingMethods::Registers_x86> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "127" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x86::EAX;
        trmaction.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                                DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                                DAddingMethods::Registers_x86::ESI };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.tramp_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x86::EDI };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::INIT;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x86>(elf, inject_desc))
            return false;
    }
    else {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x64> inject_desc;
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        DAddingMethods::OEPWrapper<DAddingMethods::Registers_x64> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "188" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x64::RAX;
        trmaction.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                                DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                                DAddingMethods::Registers_x64::R10 };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.oep_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x64::R11, DAddingMethods::Registers_x64::R12 };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::INIT;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x64>(elf, inject_desc))
            return false;
    }
    // rename file on hard drive
    QFile::rename("template", QString("_%1oinit").arg(elf_fname));
    qDebug() << "saving to file: " << QString("_%1oinit").arg(elf_fname);
    return true;
}

bool test_initarray_oep_wrappers(const QString &elf_fname, const QString &wrapper,
                                 const QString &method, const QString &handl) {
    // DAddingMethods::InjectDescription inject_desc;
    ELF elf(elf_fname);
    // qDebug() << "valid: " << elf.is_valid();
    if (!elf.is_valid())
        return false;
    QFile code;
    DAddingMethods dam;
    if (elf.is_x86()) {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x86> inject_desc;
        DAddingMethods::TrampolineWrapper<DAddingMethods::Registers_x86> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "127" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x86::EAX;
        trmaction.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                                DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                                DAddingMethods::Registers_x86::ESI };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.tramp_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x86::EDI };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::INIT_ARRAY;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x86>(elf, inject_desc))
            return false;
    }
    else {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x64> inject_desc;
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        DAddingMethods::OEPWrapper<DAddingMethods::Registers_x64> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "188" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x64::RAX;
        trmaction.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                                DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                                DAddingMethods::Registers_x64::R10 };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.oep_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x64::R11, DAddingMethods::Registers_x64::R12 };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::INIT_ARRAY;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x64>(elf, inject_desc))
            return false;
    }
    // rename file on hard drive
    QFile::rename("template", QString("_%1oinita").arg(elf_fname));
    qDebug() << "saving to file: " << QString("_%1oinita").arg(elf_fname);
    return true;
}

bool test_ctors_oep_wrappers(const QString &elf_fname, const QString &wrapper,
                             const QString &method, const QString &handl) {
    // DAddingMethods::InjectDescription inject_desc;
    ELF elf(elf_fname);
    // qDebug() << "valid: " << elf.is_valid();
    if (!elf.is_valid())
        return false;
    QFile code;
    DAddingMethods dam;
    if (elf.is_x86()) {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x86> inject_desc;
        DAddingMethods::TrampolineWrapper<DAddingMethods::Registers_x86> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x86> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "127" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x86::EAX;
        trmaction.used_regs = { DAddingMethods::Registers_x86::EAX, DAddingMethods::Registers_x86::ECX,
                                DAddingMethods::Registers_x86::EBX, DAddingMethods::Registers_x86::EDX,
                                DAddingMethods::Registers_x86::ESI };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.tramp_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x86::EDI };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::CTORS;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x86>(elf, inject_desc))
            return false;
    }
    else {
        DAddingMethods::InjectDescription<DAddingMethods::Registers_x64> inject_desc;
        inject_desc.cm = DAddingMethods::CallingMethod::OEP;
        DAddingMethods::OEPWrapper<DAddingMethods::Registers_x64> trmwrapper;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> handler;
        DAddingMethods::Wrapper<DAddingMethods::Registers_x64> trmaction;
        // debugger detection handler
        code.setFileName(handl);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        handler.params = { { "ret", "188" } };
        handler.detect_handler = nullptr;
        handler.code = code.readAll();
        // qDebug() << handler.code;
        code.close();
        // debugger detection method
        code.setFileName(method);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmaction.code = code.readAll();
        trmaction.detect_handler = nullptr;
        trmaction.ret = DAddingMethods::Registers_x64::RAX;
        trmaction.used_regs = { DAddingMethods::Registers_x64::RAX, DAddingMethods::Registers_x64::RDI,
                                DAddingMethods::Registers_x64::RSI, DAddingMethods::Registers_x64::RDX,
                                DAddingMethods::Registers_x64::R10 };
        // qDebug() << oepaction.code;
        code.close();
        // wrapper like OEP or thread
        code.setFileName(wrapper);
        if (!code.open(QIODevice::ReadOnly))
            return false;
        trmwrapper.detect_handler = &handler;
        trmwrapper.oep_action = &trmaction;
        trmwrapper.code = code.readAll();
        trmwrapper.used_regs = { DAddingMethods::Registers_x64::R11, DAddingMethods::Registers_x64::R12 };
        // qDebug() << oepwrapper.code;
        code.close();
        inject_desc.cm = DAddingMethods::CallingMethod::CTORS;
        inject_desc.adding_method = &trmwrapper;
        if (!dam.secure_elf<DAddingMethods::Registers_x64>(elf, inject_desc))
            return false;
    }
    // rename file on hard drive
    QFile::rename("template", QString("_%1octors").arg(elf_fname));
    qDebug() << "saving to file: " << QString("_%1octors").arg(elf_fname);
    return true;
}

/*
void test_wrappers() {
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for my 32-bit app...";
    // test oep + ptrace
    if (!test_oep_wrappers("my32", "oep_t.asm", "ptrace_t.asm", "exit_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for my 64-bit app...";
    if (!test_oep_wrappers("my64", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for derby 32-bit app...";
    // test oep + ptrace
    if (!test_oep_wrappers("derby32", "oep_t.asm", "ptrace_t.asm", "exit_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for derby 64-bit app...";
    if (!test_oep_wrappers("derby64", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for edb 64-bit app...";
    if (!test_oep_wrappers("edb", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ptrace for dDeflect 64-bit app...";
    if (!test_oep_wrappers("dDeflect", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ptrace for dDeflect 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for my 32-bit app...";
    // test oep + ptrace
    if (!test_thread_wrappers("my32", "threadp_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for my 64-bit app...";
    if (!test_thread_wrappers("my64", "thread64p_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for derby 32-bit app...";
    // test oep + ptrace
    if (!test_thread_wrappers("derby32", "threadp_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for derby 64-bit app...";
    if (!test_thread_wrappers("derby64", "thread64p_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for edb 64-bit app...";
    if (!test_thread_wrappers("edb", "thread64p_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + thread for dDeflect 64-bit app...";
    if (!test_thread_wrappers("dDeflect", "thread64p_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + thread for dDeflect 64-bit app done";
}
*/

void test_wrappers() {
    /*
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for my 32-bit app...";
    // test oep + ptrace
    if (!test_ctors_oep_wrappers("my32", "oep_t.asm", "ptrace_t.asm", "exit_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for my 64-bit app...";
    if (!test_ctors_oep_wrappers("my64", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for derby 32-bit app...";
    // test oep + ptrace
    if (!test_ctors_oep_wrappers("derby32", "oep_t.asm", "ptrace_t.asm", "exit_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for derby 64-bit app...";
    if (!test_ctors_oep_wrappers("derby64", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for edb 64-bit app...";
    if (!test_ctors_oep_wrappers("edb", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + ctors for dDeflect 64-bit app...";
    if (!test_ctors_oep_wrappers("dDeflect", "oep64_t.asm", "ptrace64_t.asm", "exit64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + ctors for dDeflect 64-bit app done";
    qDebug() << "=========================================";
    */
    qDebug() << "Testing OEP + init for my 32-bit app...";
    // test oep + ptrace
    if (!test_init_oep_wrappers("my32", "oep_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + init for my 64-bit app...";
    if (!test_init_oep_wrappers("my64", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + init for derby 32-bit app...";
    // test oep + ptrace
    if (!test_init_oep_wrappers("derby32", "oep_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + init for derby 64-bit app...";
    if (!test_init_oep_wrappers("derby64", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + init for edb 64-bit app...";
    if (!test_init_oep_wrappers("edb", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + init for dDeflect 64-bit app...";
    if (!test_init_oep_wrappers("dDeflect", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + init for dDeflect 64-bit app done";
    /*
    qDebug() << "Testing OEP + initarray for my 32-bit app...";
    // test oep + ptrace
    if (!test_initarray_oep_wrappers("my32", "oep_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for my 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + initarray for my 64-bit app...";
    if (!test_initarray_oep_wrappers("my64", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for my 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + initarray for derby 32-bit app...";
    // test oep + ptrace
    if (!test_initarray_oep_wrappers("derby32", "oep_t.asm", "ptrace_t.asm", "exit_group_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for derby 32-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + initarray for derby 64-bit app...";
    if (!test_initarray_oep_wrappers("derby64", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for derby 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + initarray for edb 64-bit app...";
    if (!test_initarray_oep_wrappers("edb", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for edb 64-bit app done";
    qDebug() << "=========================================";
    qDebug() << "Testing OEP + initarray for dDeflect 64-bit app...";
    if (!test_initarray_oep_wrappers("dDeflect", "oep64_t.asm", "ptrace64_t.asm", "exit_group64_t.asm")) {
        qDebug() << "something went wrong :(";
    }
    qDebug() << "Testing OEP + initarray for dDeflect 64-bit app done";
    */
}