#ifndef APPLICATIONMANAGER_H
#define APPLICATIONMANAGER_H

#include <QObject>
#include <QDir>
#include <QDirIterator>

#include <core/adding_methods/wrappers/linux/daddingmethods.h>
#include <ApplicationManager/DJsonParser/djsonparser.h>
#include <ApplicationManager/DSourceCodeParser/dsourcecodeparser.h>

class ApplicationManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QVariantList x86MethodsNames READ x86MethodsNames NOTIFY x86MethodsNamesChanged)
public:
    template <typename Reg>
    using IDList = QList<DAddingMethods::InjectDescription<Reg>*>;
    template <typename Reg>
    using FIDMapping = QHash<QString, IDList<Reg>>;

    explicit ApplicationManager(QObject *parent = 0);
    virtual ~ApplicationManager();
    enum State{IDLE, EXEC, SOURCE};

    void setState(State state);
    State state() const;

    QVariantList x86MethodsNames();
signals:
    void stateChanged(State);
    void x86MethodsNamesChanged();
public slots:
    void fileOpened(QString);
    void insertMethods(FIDMapping<DAddingMethods::Registers_x86>);
private:
    State m_state;
    IDList<DAddingMethods::Registers_x86> m_x86methodsList;
    QVariantList m_x86MethodsNames;
    IDList<DAddingMethods::Registers_x64> m_x64methodsList;
    QVariantList m_x64MethodsNames;
    QString m_targetPath;
    QStringList sourceExtensionList;
    DJsonParser jsonParser;
    DSourceCodeParser *sourceParser;
};

#endif // APPLICATIONMANAGER_H