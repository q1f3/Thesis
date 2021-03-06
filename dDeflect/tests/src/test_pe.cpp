#include "test_pe.h"

#include <QFile>
#include <QDir>
#include <QByteArray>
#include <QList>
#include <core/file_types/pefile.h>
#include <core/adding_methods/wrappers/peaddingmethods.h>
#include <ApplicationManager/DJsonParser/djsonparser.h>
#include <ApplicationManager/dsettings.h>
#include <ApplicationManager/dlogger.h>

QList<QString> PETester::methods_x64 =
{
    "win_x64_anti_step_over",
    "win_x64_anti_step_over2",
    "win_x64_anti_step_over3",
    "win_x64_being_debugged",
    "win_x64_check_remote_debugger_present",
    "win_x64_dbg_break_point",
    "win_x64_find_window",
    "win_x64_heap_flags",
    "win_x64_is_debugger_present",
    "win_x64_nt_global_flag",
    "win_x64_nt_set_information_thread",
    "win_x64_open_process",
    "win_x64_parent_process",
    "win_x64_process_debug_flags",
    "win_x64_process_debug_object_handle",
    "win_x64_process_debug_port",
    "win_x64_tls_ep_breakpoint"
};

QList<QString> PETester::methods_x86 =
{
    "win_x86_anti_step_over",
    "win_x86_anti_step_over2",
    "win_x86_anti_step_over3",
    "win_x86_being_debugged",
    "win_x86_check_remote_debugger_present",
    "win_x86_dbg_break_point",
    "win_x86_find_window",
    "win_x86_heap_flags",
    "win_x86_is_debugger_present",
    "win_x86_nt_global_flag",
    "win_x86_nt_set_information_thread",
    "win_x86_open_process",
    "win_x86_parent_process",
    "win_x86_process_debug_flags",
    "win_x86_process_debug_object_handle",
    "win_x86_process_debug_port",
    "win_x86_tls_ep_breakpoint",
    "win_x86_fld",
    "win_x86_create_file"
};

QList<QString> PETester::handlers_x86 =
{
    "win_x86_handler_abort",
    "win_x86_handler_assert",
    "win_x86_handler_divide",
    "win_x86_handler_exit",
    "win_x86_handler_exit_process",
    "win_x86_handler_loop",
    "win_x86_handler_memleak",
    "win_x86_handler_message_box",
    "win_x86_handler_sigsegv",
    "win_x86_handler_stack",
    "win_x86_handler_ud2"
};

QList<QString> PETester::handlers_x64 =
{
    "win_x64_handler_abort",
    "win_x64_handler_assert",
    "win_x64_handler_divide",
    "win_x64_handler_exit",
    "win_x64_handler_exit_process",
    "win_x64_handler_loop",
    "win_x64_handler_memleak",
    "win_x64_handler_message_box",
    "win_x64_handler_sigsegv",
    "win_x64_handler_stack",
    "win_x64_handler_ud2"
};

PETester::PETester()
{
}

bool PETester::test_one(QString input, QString output, PETester::Method type, QString method, QString handler)
{
    QDir().mkdir("tests_output");

    QFile in(input);
    if(!in.open(QFile::ReadOnly))
        return false;

    QByteArray data = in.readAll();

    PEFile pe(data);

    if(!pe.is_valid())
        return false;

    if(pe.is_x64() && !test_one_ex<Registers_x64>(&pe, type, method, handler))
        return false;

    if(pe.is_x86() && !test_one_ex<Registers_x86>(&pe, type, method, handler))
        return false;

    QFile out(QFileInfo(QString("tests_output"), output).absoluteFilePath());
    if(!out.open(QFile::WriteOnly))
        return false;

    out.write(pe.getData());

    out.close();
    in.close();

    return true;
}

bool PETester::test_all_methods(QString input, Method type, QString handler)
{
    QFile in(input);
    if(!in.open(QFile::ReadOnly))
        return false;

    QByteArray data = in.readAll();

    PEFile pe(data);
    if(!pe.is_valid())
        return false;

    bool is_x64 = pe.is_x64();

    in.close();

    QList<QString> *methods = is_x64 ? &methods_x64 : &methods_x86;

    int i = 1;
    foreach(QString method, *methods)
    {
        LOG_MSG(QString("\nStarting test %1 / %2...").arg(i).arg(methods->length()));
        QString t = type == Method::EntryPoint ? "EntryPoint" : (type == Method::TLS ? "TLS" : "Trampoline");
        QString output = QString("test_method_%1_%2_%3.exe").arg(method.split('.')[0]).arg(t).arg(handler.split('.')[0]);
        if(!test_one(input, output, type, method, handler))
            return false;

        LOG_MSG(QString("Test %1 / %2 finished.").arg(i).arg(methods->length()));

        ++i;
    }

    return true;
}

bool PETester::test_all_handlers(QString input, Method type, QString method)
{
    QFile in(input);
    if(!in.open(QFile::ReadOnly))
        return false;

    QByteArray data = in.readAll();

    PEFile pe(data);
    if(!pe.is_valid())
        return false;

    bool is_x64 = pe.is_x64();

    in.close();

    QList<QString> *handlers = is_x64 ? &handlers_x64 : &handlers_x86;

    int i = 1;
    foreach(QString handler, *handlers)
    {
        LOG_MSG(QString("\nStarting test %1 / %2...").arg(i).arg(handlers->length()));
        QString t = type == Method::EntryPoint ? "EntryPoint" : (type == Method::TLS ? "TLS" : "Trampoline");
        QString output = QString("test_handler_%1_%2_%3.exe").arg(method.split('.')[0]).arg(t).arg(handler.split('.')[0]);
        if(!test_one(input, output, type, method, handler))
            return false;

        LOG_MSG(QString("Test %1 / %2 finished.").arg(i).arg(handlers->length()));

        ++i;
    }

    return true;
}

bool PETester::test_thread(QString input, QString output, QString method, QString handler)
{
    QDir().mkdir("tests_output");

    QFile in(input);
    if(!in.open(QFile::ReadOnly))
        return false;

    QByteArray data = in.readAll();

    PEFile pe(data);

    if(!pe.is_valid())
        return false;

    if(pe.is_x64() && !test_thread_ex<Registers_x64>(&pe, method, handler))
        return false;

    if(pe.is_x86() && !test_thread_ex<Registers_x86>(&pe, method, handler))
        return false;

    QFile out(QFileInfo(QString("tests_output"), output).absoluteFilePath());
    if(!out.open(QFile::WriteOnly))
        return false;

    out.write(pe.getData());

    out.close();
    in.close();

    return true;
}

bool PETester::test_everything(QString input)
{
    int errors = 0;

    LOG_MSG("Testing methods [EntryPoint]...");
    if(test_all_methods(input, PETester::Method::EntryPoint, "win_x86_handler_message_box"))
        LOG_MSG("Tests done (methods) [EntryPoint]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    LOG_MSG("Testing methods [Trampoline]...");
    if(test_all_methods(input, PETester::Method::Trampoline, "win_x86_handler_message_box"))
        LOG_MSG("Tests done (methods) [Trampoline]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    LOG_MSG("Testing methods [TLS]...");
    if(test_all_methods(input, PETester::Method::TLS, "win_x86_handler_message_box"))
        LOG_MSG("Tests done (methods) [TLS]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }


    LOG_MSG("Testing handlers [EntryPoint]...");
    if(test_all_handlers(input, PETester::Method::EntryPoint, "win_x86_is_debugger_present"))
        LOG_MSG("Tests done (handlers) [EntryPoint]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    LOG_MSG("Testing handlers [Trampoline]...");
    if(test_all_handlers(input, PETester::Method::Trampoline, "win_x86_is_debugger_present"))
        LOG_MSG("Tests done (handlers) [Trampoline]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    LOG_MSG("Testing handlers [TLS]...");
    if(test_all_handlers(input, PETester::Method::TLS, "win_x86_is_debugger_present"))
        LOG_MSG("Tests done (handlers) [TLS]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    LOG_MSG("Testing thread...");
    if(test_thread(input, QString("test_thread_win_x86_is_debugger_present_win_x86_handler_message_box.exe"), "win_x86_is_debugger_present", "win_x86_handler_message_box"))
         LOG_MSG("Tests done [Thread]!");
    else
    {
        errors++;
        LOG_MSG("Tests failed!");
    }

    if(errors)
        LOG_MSG(QString("%1 tests failed!").arg(errors));

    return errors == 0;
}

template <typename Reg>
bool PETester::test_one_ex(PEFile *pe, PETester::Method type, QString method, QString handler)
{
    DJsonParser parser(DSettings::getSettings().getDescriptionsPath<Reg>());

    PEAddingMethods<Reg> adder(pe);

     Wrapper<Reg> *meth = parser.loadInjectDescription<Reg>(QString("%1.json").arg(method));
    if(!meth)
        return false;

    if(meth->ret != Reg::None)
    {
        meth->detect_handler = parser.loadInjectDescription<Reg>(QString("%1.json").arg(handler));

        if(!meth->detect_handler)
        {
            delete meth;
            return false;
        }
    }

    typename DAddingMethods<Reg>::InjectDescription id;
    id.adding_method = meth;
    id.cm = static_cast<typename DAddingMethods<Reg>::CallingMethod>(type);
    QList<typename DAddingMethods<Reg>::InjectDescription*> ids = { &id };

    adder.secure(ids);

    if(!meth->detect_handler)
        delete meth->detect_handler;

    delete meth;

    return true;
}

template <typename Reg>
bool PETester::test_thread_ex(PEFile *pe, QString method, QString handler)
{
    DJsonParser parser(DSettings::getSettings().getDescriptionsPath<Reg>());

    PEAddingMethods<Reg> adder(pe);

    QString tname = QString("win_%1_helper_create_thread.json").arg(pe->is_x64() ? "x64" : "x86");
    ThreadWrapper<Reg> *tmeth =
            dynamic_cast<ThreadWrapper<Reg>*>(parser.loadInjectDescription<Reg>(tname));
    if(!tmeth)
        return false;

     Wrapper<Reg> *meth = parser.loadInjectDescription<Reg>(QString("%1.json").arg(method));
    if(!meth)
    {
        delete tmeth;
        return false;
    }

    if(meth->ret != Reg::None)
    {
        meth->detect_handler = parser.loadInjectDescription<Reg>(QString("%1.json").arg(handler));

        if(!meth->detect_handler)
        {
            delete meth;
            delete tmeth;
            return false;
        }
    }

    tmeth->thread_actions.append(meth);

    typename DAddingMethods<Reg>::InjectDescription id;
    id.adding_method = tmeth;
    id.cm = DAddingMethods<Reg>::CallingMethod::OEP;
    QList<typename DAddingMethods<Reg>::InjectDescription*> ids = { &id };

    adder.secure(ids);

    if(!meth->detect_handler)
        delete meth->detect_handler;

    delete meth;
    delete tmeth;

    return true;
}
