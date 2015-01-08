#ifndef ELFADDINGMETHODS_H
#define ELFADDINGMETHODS_H

#include <QTemporaryFile>
#include <QFileInfo>

#include <core/adding_methods/wrappers/daddingmethods.h>

template <typename RegistersType>
class ELFAddingMethods : public DAddingMethods {
public:
    ELFAddingMethods(ELF *f);
    ~ELFAddingMethods();

    /**
     * @brief Metoda zabezpiecza plik, podany jako argument za pomocą wyspecyfikowanej metody.
     * @param elf plik do zabezpieczania.
     * @param inject_desc opis metody wstrzykiwania kodu.
     * @return True, jeżeli operacja się powiodła, False w innych przypadkach.
     */
    bool secure(const QList<InjectDescription<RegistersType>*> &inject_desc);

private:
    enum class PlaceholderMnemonics {
        DDETECTIONHANDLER,
        DDETECTIONMETHOD,
        DDRET
    };

    /**
     * @brief Typy placeholderów.
     */
    enum class PlaceholderTypes {
        PARAM_PRE,
        PARAM_POST,
        PLACEHOLDER_PRE,
        PLACEHOLDER_POST
    };

    QMap<PlaceholderTypes, QString> placeholder_id;
    QMap<PlaceholderMnemonics, QString> placeholder_mnm;

    /**
     * @brief Metoda odpowiada za generowanie kodu dla dowolnego opakowania.
     * @param wrap klasa opisująca kawałek kodu do wygenerowania.
     * @param code wygenerowany kod.
     * @return True, jeżeli operacja się powiodła, False w innych przypadkach.
     */
    bool wrapper_gen_code(Wrapper<RegistersType> *wrap, QString &code);

    /**
     * @brief Metoda odpowiada za wypełnianie parametrów w podanym kodzie.
     * @param code kod.
     * @param params parametry.
     * @return ilośc zamienionych parametrów.
     */
    uint64_t fill_params(QString &code, const QMap<QString, QString> &params);

    /**
     * @brief Metoda odpowiada za wypełnianie placeholdera w podanym kodzie, za pomocą podanego kodu.
     * @param code kod.
     * @param gen_code kod, którym zostanie zamieniony placeholder.
     * @param plc_mnm placeholder.
     * @return ilośc zamienionych parametrów.
     */
    uint64_t fill_placeholders(QString &code, const QString &gen_code, PlaceholderMnemonics plc_mnm);

    /**
     * @brief Metoda odpowiada za kompilację kodu źródłowego assembly.
     * @param code2compile kod, który musi zostać skompilowany.
     * @param compiled_code skompilowany kod.
     * @return True, jeżeli operacja się powiodła, False w innych przypadkach.
     */
    bool compile(const QString &code2compile, QByteArray &compiled_code);

    /**
     * @brief Metoda odpowiada za pobieranie adresów z wyspecyfikowanych danych.
     * @param data tablica z adresami.
     * @param addr_size wielkość adresu w bajtach.
     * @param addr_list lista adresów.
     * @param except_list lista adresów, które nie trzeba dołączać do listy wynikowej.
     * @return True, jeżeli operacja się powiodła, False w innych przypadkach.
     */
    bool get_addresses(const QByteArray &data, uint8_t addr_size, QList<Elf64_Addr> &addr_list,
                       const QList<Elf64_Addr> &except_list);

    /**
     * @brief Metoda odpowiada za generowanie kodu dla funkcji, która zmienia prawa dostępu do strony pamięci.
     * @param vaddr adres wirtualny.
     * @param mem_size wielkość pamięci.
     * @param flags prawa dostępu do pamięci.
     * @param code wygenerowany kod.
     * @return True, jeżeli operacja się powiodła, False w innych przypadkach.
     */
    template <typename AddrT, typename XwordT, typename WordT>
    bool set_prot_flags_gen_code(AddrT vaddr, XwordT mem_size, WordT flags, QString &code);

    /**
     * @brief Metoda odpowiada za pobieranie offsetow instrukcji w pliku.
     * @param opcodes lista instrukcji.
     * @param file_off offset w pliku.
     * @param base_off wartość bazowa offsetu.
     */
    void get_file_offsets_from_opcodes(QStringList &opcodes, QList<Elf64_Addr> &file_off, Elf64_Addr base_off);
};

#endif // ELFADDINGMETHODS_H
