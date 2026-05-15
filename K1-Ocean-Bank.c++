// Kelompok 1 - Praktikum Pemrograman Berorientasi Objek
// 1. Khairunnisa - 251402017
// 2. Muhammad Ihsan Anwar - 251402044
// 3. Nadia Stevany Br Situmorang - 251402073
// 4. Viter Moldy Kesuma - 251402079
// 5. Agnes Natalia Br Siregar - 251402108
// 6. Limjun Basani Sipayung - 251402144

// SISTEM OCEAN BANK

#include <iostream>   // Input dan output data ke terminal
#include <fstream>    // Membaca dan menulis file nasabah.txt
#include <vector>     // Menyimpan daftar nasabah dalam bentuk array dinamis
#include <iomanip>    // Mengatur format tampilan angka dan teks
#include <sstream>    // Memecah isi baris file berdasarkan pemisah |
#include <memory>     // Menggunakan unique_ptr untuk polymorphism yang aman
#include <algorithm>  // Fitur sort, remove_if, min, dan utilitas algoritma
#include <limits>     // Batas maksimal stream untuk membersihkan input
#include <windows.h>  // Memberi warna pada teks output terminal Windows
using namespace std;

// ================= HELPER =================
enum ConsoleColor {
    COLOR_DEFAULT = 7,
    COLOR_BLUE = 9,
    COLOR_GREEN = 10,
    COLOR_CYAN = 11,
    COLOR_RED = 12,
    COLOR_YELLOW = 14,
    COLOR_WHITE = 15
};

void setColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor() {
    setColor(COLOR_DEFAULT);
}

void printColored(const string &teks, WORD color, bool newline = true) {
    setColor(color);
    cout << teks;
    if (newline) {
        cout << endl;
    }
    resetColor();
}

vector<string> split(const string &teks, char pemisah) {
    vector<string> bagian;
    string token;
    stringstream ss(teks);

    while (getline(ss, token, pemisah)) {
        bagian.push_back(token);
    }

    return bagian;
}

// ================= ABSTRACT CLASS =================
class BankAccount {
protected:
    string nama;
    string spesies;
    string rekening;
    double saldo;
    bool aktif;

public:
    BankAccount(string nama, string spesies, string rekening, double saldo)
        : nama(nama), spesies(spesies), rekening(rekening), saldo(saldo), aktif(true) {}

    virtual ~BankAccount() {}

    string getNama() const {
        return nama;
    }

    string getSpesies() const {
        return spesies;
    }

    string getRekening() const {
        return rekening;
    }

    double getSaldo() const {
        return saldo;
    }

    bool isAktif() const {
        return aktif;
    }

    void nonaktifkan() {
        aktif = false;
    }

    virtual int getId() const = 0;
    virtual string getJenisAkun() const = 0;
    virtual double hitungBungaBulanan() const = 0;
    virtual double potongBiayaAdmin() = 0;
    virtual bool setor(double jumlah) = 0;
    virtual bool tarik(double jumlah) = 0;
    virtual void tampilkanData() const = 0;
    virtual string serialize() const = 0;

    double prosesBungaBulanan() {
        if (!aktif) {
            return 0.0;
        }

        double bunga = hitungBungaBulanan();
        saldo += bunga;
        return bunga;
    }
};

// ================= CLASS NASABAH REGULER =================
class Nasabah : public BankAccount {
private:
    int id;
    double biayaAdminBulanan;

public:
    Nasabah(int id, string nama, string spesies, string rekening, double saldo,
            double biayaAdminBulanan = 2500.0)
        : BankAccount(nama, spesies, rekening, saldo),
          id(id),
          biayaAdminBulanan(biayaAdminBulanan) {}

    ~Nasabah() override {}

    int getId() const override {
        return id;
    }

    string getJenisAkun() const override {
        return "REGULER";
    }

    double hitungBungaBulanan() const override {
        return saldo * 0.005;
    }

    double potongBiayaAdmin() override {
        if (!aktif) {
            return 0.0;
        }

        double potongan = min(saldo, biayaAdminBulanan);
        saldo -= potongan;
        return potongan;
    }

    bool setor(double jumlah) override {
        if (!aktif || jumlah <= 0) {
            return false;
        }

        saldo += jumlah;
        return true;
    }

    bool tarik(double jumlah) override {
        if (!aktif || jumlah <= 0 || jumlah > saldo) {
            return false;
        }

        saldo -= jumlah;
        return true;
    }

    void tampilkanData() const override {
        setColor(COLOR_CYAN);
        cout << "====================================" << endl;
        cout << "ID            : "; setColor(COLOR_WHITE); cout << id << endl;
        setColor(COLOR_CYAN);
        cout << "Jenis Akun    : "; setColor(COLOR_YELLOW); cout << getJenisAkun() << endl;
        setColor(COLOR_CYAN);
        cout << "Nama          : "; setColor(COLOR_WHITE); cout << nama << endl;
        setColor(COLOR_CYAN);
        cout << "Spesies       : "; setColor(COLOR_WHITE); cout << spesies << endl;
        setColor(COLOR_CYAN);
        cout << "No Rekening   : "; setColor(COLOR_WHITE); cout << rekening << endl;
        setColor(COLOR_CYAN);
        cout << "Status        : "; setColor(aktif ? COLOR_GREEN : COLOR_RED); cout << (aktif ? "AKTIF" : "NONAKTIF") << endl;
        setColor(COLOR_CYAN);
        cout << "Saldo         : "; setColor(COLOR_GREEN); cout << fixed << setprecision(2) << saldo << endl;
        setColor(COLOR_CYAN);
        cout << "====================================" << endl;
        resetColor();
    }

    string serialize() const override {
        stringstream ss;
        ss << id << "|"
           << nama << "|"
           << spesies << "|"
           << rekening << "|"
           << fixed << setprecision(2) << saldo << "|"
           << getJenisAkun() << "|"
           << (aktif ? 1 : 0);
        return ss.str();
    }

    // Operator overloading untuk cek rekening cepat.
    bool operator==(const string &noRek) const {
        return rekening == noRek;
    }

    bool operator<(const Nasabah &lain) const {
        return saldo < lain.saldo;
    }

    friend ostream &operator<<(ostream &os, const Nasabah &n) {
        os << "[" << n.getJenisAkun() << "] "
           << "ID " << n.getId() << " - "
           << n.getNama() << " (" << n.getRekening() << ")"
           << " Saldo: " << fixed << setprecision(2) << n.getSaldo();
        return os;
    }
};

// ================= CLASS PREMIUM =================
class PremiumNasabah : public Nasabah {
private:
    double bonusSetoranRate;

public:
    PremiumNasabah(int id, string nama, string spesies,
                   string rekening, double saldo, double bonusSetoranRate = 0.01)
        : Nasabah(id, nama, spesies, rekening, saldo, 0.0),
          bonusSetoranRate(bonusSetoranRate) {}

    ~PremiumNasabah() override {}

    string getJenisAkun() const override {
        return "PREMIUM";
    }

    double hitungBungaBulanan() const override {
        return getSaldo() * 0.012;
    }

    double potongBiayaAdmin() override {
        return 0.0;
    }

    bool setor(double jumlah) override {
        if (!Nasabah::setor(jumlah)) {
            return false;
        }

        // Premium dapat cashback dari nominal setoran.
        return Nasabah::setor(jumlah * bonusSetoranRate);
    }

    void tampilkanData() const override {
        setColor(COLOR_BLUE);
        cout << "====================================" << endl;
        cout << "ID            : "; setColor(COLOR_WHITE); cout << getId() << endl;
        setColor(COLOR_BLUE);
        cout << "Jenis Akun    : "; setColor(COLOR_YELLOW); cout << getJenisAkun() << endl;
        setColor(COLOR_BLUE);
        cout << "Nama          : "; setColor(COLOR_WHITE); cout << getNama() << endl;
        setColor(COLOR_BLUE);
        cout << "Spesies       : "; setColor(COLOR_WHITE); cout << getSpesies() << endl;
        setColor(COLOR_BLUE);
        cout << "No Rekening   : "; setColor(COLOR_WHITE); cout << getRekening() << endl;
        setColor(COLOR_BLUE);
        cout << "Status        : "; setColor(isAktif() ? COLOR_GREEN : COLOR_RED); cout << (isAktif() ? "AKTIF" : "NONAKTIF") << endl;
        setColor(COLOR_BLUE);
        cout << "Saldo         : "; setColor(COLOR_GREEN); cout << fixed << setprecision(2) << getSaldo() << endl;
        setColor(COLOR_BLUE);
        cout << "Bonus Setoran : "; setColor(COLOR_WHITE); cout << bonusSetoranRate * 100 << "%" << endl;
        cout << "====================================" << endl;
        resetColor();
    }
};

// ================= CLASS BANK SYSTEM =================
class BankSystem {
private:
    vector<unique_ptr<BankAccount>> daftarNasabah;
    string filePath;
    int idCounter;

    BankAccount *cariByRekening(const string &rekening) {
        // First try match by rekening string
        for (auto &n : daftarNasabah) {
            if (n->getRekening() == rekening) {
                return n.get();
            }
        }

        // If not found, try interpret input as numeric ID
        try {
            int id = stoi(rekening);
            for (auto &n : daftarNasabah) {
                if (n->getId() == id) return n.get();
            }
        } catch (...) {
            // ignore parse errors
        }

        return nullptr;
    }

    const BankAccount *cariByRekening(const string &rekening) const {
        for (const auto &n : daftarNasabah) {
            if (n->getRekening() == rekening) {
                return n.get();
            }
        }

        try {
            int id = stoi(rekening);
            for (const auto &n : daftarNasabah) {
                if (n->getId() == id) return n.get();
            }
        } catch (...) {}

        return nullptr;
    }

public:
    BankSystem(string filePath)
        : filePath(filePath), idCounter(1) {
        muatDariFile();
    }

    ~BankSystem() {
        simpanSemuaKeFile();
    }

    bool rekeningSudahAda(const string &rekening) const {
        return cariByRekening(rekening) != nullptr;
    }

    void muatDariFile() {
        ifstream file(filePath);
        if (!file.is_open()) {
            ofstream buatFile(filePath);
            buatFile.close();
            return;
        }

        string baris;
        int maxId = 0;

        while (getline(file, baris)) {
            if (baris.empty()) {
                continue;
            }

            vector<string> kolom = split(baris, '|');
            if (kolom.size() < 5) {
                continue;
            }

            int id;
            double saldo;

            try {
                id = stoi(kolom[0]);
                saldo = stod(kolom[4]);
            } catch (...) {
                continue;
            }

            string nama = kolom[1];
            string spesies = kolom[2];
            string rekening = kolom[3];

            string jenis = "REGULER";
            bool aktif = true;

            for (size_t i = 5; i < kolom.size(); i++) {
                if (kolom[i] == "REGULER" || kolom[i] == "PREMIUM") {
                    jenis = kolom[i];
                    break;
                }
            }

            for (int i = static_cast<int>(kolom.size()) - 1; i >= 5; i--) {
                if (kolom[i] == "1" || kolom[i] == "0") {
                    aktif = (kolom[i] == "1");
                    break;
                }
            }

            if (rekeningSudahAda(rekening)) {
                continue;
            }

            if (jenis == "PREMIUM") {
                daftarNasabah.push_back(
                    make_unique<PremiumNasabah>(id, nama, spesies, rekening, saldo));
            } else {
                daftarNasabah.push_back(
                    make_unique<Nasabah>(id, nama, spesies, rekening, saldo));
            }

            if (!aktif) {
                daftarNasabah.back()->nonaktifkan();
            }

            maxId = max(maxId, id);
        }

        idCounter = maxId + 1;
    }

    void simpanSemuaKeFile() const {
        ofstream file(filePath, ios::trunc);
        for (const auto &n : daftarNasabah) {
            file << n->serialize() << endl;
        }
    }

    bool tambahNasabah(const string &nama, const string &spesies,
                      const string &rekening, double saldoAwal,
                      bool premium) {
        if (rekeningSudahAda(rekening) || saldoAwal < 0) {
            return false;
        }

        if (premium) {
            daftarNasabah.push_back(
                make_unique<PremiumNasabah>(idCounter, nama, spesies, rekening, saldoAwal));
        } else {
            daftarNasabah.push_back(
                make_unique<Nasabah>(idCounter, nama, spesies, rekening, saldoAwal));
        }

        idCounter++;
        simpanSemuaKeFile();
        return true;
    }

    bool setor(const string &rekening, double jumlah) {
        BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            return false;
        }

        bool sukses = akun->setor(jumlah);
        if (sukses) {
            simpanSemuaKeFile();
        }
        return sukses;
    }

    bool tarik(const string &rekening, double jumlah) {
        BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            return false;
        }

        bool sukses = akun->tarik(jumlah);
        if (sukses) {
            simpanSemuaKeFile();
        }
        return sukses;
    }

    bool transfer(const string &rekeningAsal, const string &rekeningTujuan, double jumlah) {
        if (rekeningAsal == rekeningTujuan || jumlah <= 0) {
            return false;
        }

        BankAccount *asal = cariByRekening(rekeningAsal);
        BankAccount *tujuan = cariByRekening(rekeningTujuan);

        if (asal == nullptr || tujuan == nullptr) {
            return false;
        }

        const double biayaTransfer = 2500.0;
        if (!asal->tarik(jumlah + biayaTransfer)) {
            return false;
        }

        if (!tujuan->setor(jumlah)) {
            asal->setor(jumlah + biayaTransfer);
            return false;
        }

        simpanSemuaKeFile();
        return true;
    }

    bool nonaktifkanRekening(const string &rekening) {
        BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr || !akun->isAktif()) {
            return false;
        }

        akun->nonaktifkan();
        simpanSemuaKeFile();
        return true;
    }

    bool hapusNasabah(const string &rekening) {
        auto it = remove_if(daftarNasabah.begin(), daftarNasabah.end(),
                            [&](const unique_ptr<BankAccount> &n) {
                                return n->getRekening() == rekening;
                            });

        if (it == daftarNasabah.end()) {
            return false;
        }

        daftarNasabah.erase(it, daftarNasabah.end());
        simpanSemuaKeFile();
        return true;
    }

    void cekSaldo(const string &rekening) const {
        const BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            cout << "Data tidak ditemukan!" << endl;
            return;
        }

        cout << "Saldo saat ini: " << fixed << setprecision(2) << akun->getSaldo() << endl;
    }

    void tampilkanDataNasabah(const string &rekening) const {
        const BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            cout << "Data tidak ditemukan!" << endl;
            return;
        }

        akun->tampilkanData();
    }

    void laporanKeuangan() const {
        double totalSaldo = 0.0;
        int totalAktif = 0;

        cout << "\n========== LAPORAN KEUANGAN ==========" << endl;
        for (const auto &n : daftarNasabah) {
            n->tampilkanData();
            totalSaldo += n->getSaldo();
            if (n->isAktif()) {
                totalAktif++;
            }
        }

        cout << "Total Nasabah         : " << daftarNasabah.size() << endl;
        cout << "Total Nasabah Aktif   : " << totalAktif << endl;
        cout << "Total Dana Tersimpan  : " << fixed << setprecision(2) << totalSaldo << endl;
    }

    void prosesAkhirBulan() {
        double totalBunga = 0.0;
        double totalAdmin = 0.0;

        for (auto &n : daftarNasabah) {
            totalBunga += n->prosesBungaBulanan();
            totalAdmin += n->potongBiayaAdmin();
        }

        simpanSemuaKeFile();

        cout << "\nProses akhir bulan selesai." << endl;
        cout << "Total bunga dibagikan : " << fixed << setprecision(2) << totalBunga << endl;
        cout << "Total admin dipotong  : " << fixed << setprecision(2) << totalAdmin << endl;
    }

    void tampilkanTopSaldo(int jumlahTop = 3) const {
        vector<const BankAccount *> data;

        for (const auto &n : daftarNasabah) {
            data.push_back(n.get());
        }

        sort(data.begin(), data.end(), [](const BankAccount *a, const BankAccount *b) {
            return a->getSaldo() > b->getSaldo();
        });

        cout << "\n========== TOP NASABAH BERDASARKAN SALDO ==========" << endl;
        int batas = min(static_cast<int>(data.size()), jumlahTop);

        for (int i = 0; i < batas; i++) {
            const Nasabah *n = dynamic_cast<const Nasabah *>(data[i]);
            if (n != nullptr) {
                cout << i + 1 << ". " << *n << endl;
            }
        }
    }
};

// ================= INPUT HELPER =================
int inputAngka(const string &label) {
    int nilai;

    while (true) {
        cout << label;
        if (cin >> nilai) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return nilai;
        }

        if (cin.eof()) {
            return 12;
        }

        printColored("Input harus berupa angka!", COLOR_RED);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double inputNominal(const string &label) {
    double nilai;

    while (true) {
        cout << label;
        if (cin >> nilai) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return nilai;
        }

        if (cin.eof()) {
            return 0.0;
        }

        printColored("Input harus berupa angka!", COLOR_RED);
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string inputTeks(const string &label) {
    string nilai;
    cout << label;
    getline(cin, nilai);
    return nilai;
}

// ================= FUNCTION =================
void tampilMenu() {
    setColor(COLOR_CYAN);
    cout << "\n========== OCEAN BANK ==========" << endl;
    setColor(COLOR_WHITE);
    cout << "1. Input Data Nasabah" << endl;
    cout << "2. Cek Saldo" << endl;
    cout << "3. Setor Saldo" << endl;
    cout << "4. Tarik Saldo" << endl;
    cout << "5. Transfer Antar Rekening" << endl;
    cout << "6. Laporan Keuangan" << endl;
    cout << "7. Cek Data Nasabah" << endl;
    cout << "8. Proses Akhir Bulan (Bunga + Admin)" << endl;
    cout << "9. Nonaktifkan Rekening" << endl;
    cout << "10. Tampilkan Top Saldo" << endl;
    cout << "11. Hapus Nasabah" << endl;
    setColor(COLOR_YELLOW);
    cout << "12. Keluar" << endl;
    setColor(COLOR_CYAN);
    cout << "Pilih Menu : ";
    resetColor();
}

// ================= MAIN =================
int main(int argc, char *argv[]) {
    string lokasiData = "output/nasabah.txt";

    if (argc > 0) {
        string lokasiExe = argv[0];
        size_t pos = lokasiExe.find_last_of("\\/");
        if (pos != string::npos) {
            lokasiData = lokasiExe.substr(0, pos + 1) + "nasabah.txt";
        }
    }

    BankSystem bank(lokasiData);
    int pilihan;

    do {
        tampilMenu();
        pilihan = inputAngka("");

        switch (pilihan) {
        case 1: {
            string nama = inputTeks("\nNama Nasabah      : ");
            string spesies = inputTeks("Spesies Laut      : ");
            string rekening = inputTeks("Nomor Rekening    : ");

            if (bank.rekeningSudahAda(rekening)) {
                printColored("Nomor rekening sudah terdaftar!", COLOR_RED);
                break;
            }

            double saldoAwal = inputNominal("Saldo Awal        : ");
            int jenis = inputAngka("Jenis Akun (1 Reguler, 2 Premium): ");

            bool premium = (jenis == 2);
            if (bank.tambahNasabah(nama, spesies, rekening, saldoAwal, premium)) {
                printColored("Nasabah berhasil ditambahkan.", COLOR_GREEN);
            } else {
                printColored("Gagal menambahkan nasabah.", COLOR_RED);
            }
            break;
        }

        case 2: {
            string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
            bank.cekSaldo(rekening);
            break;
        }

        case 3: {
            string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
            double jumlah = inputNominal("Jumlah Setor            : ");

            if (bank.setor(rekening, jumlah)) {
                printColored("Setor saldo berhasil.", COLOR_GREEN);
            } else {
                printColored("Setor gagal. Cek data rekening atau nominal.", COLOR_RED);
            }
            break;
        }

        case 4: {
            string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
            double jumlah = inputNominal("Jumlah Tarik            : ");

            if (bank.tarik(rekening, jumlah)) {
                printColored("Tarik saldo berhasil.", COLOR_GREEN);
            } else {
                printColored("Tarik gagal. Saldo tidak cukup / rekening tidak valid.", COLOR_RED);
            }
            break;
        }

        case 5: {
            string asal = inputTeks("\nRekening Asal (nomor atau ID): ");
            string tujuan = inputTeks("Rekening Tujuan (nomor atau ID): ");
            double jumlah = inputNominal("Nominal Transfer: ");

            if (bank.transfer(asal, tujuan, jumlah)) {
                printColored("Transfer berhasil (biaya transfer 2500).", COLOR_GREEN);
            } else {
                printColored("Transfer gagal. Cek rekening atau saldo.", COLOR_RED);
            }
            break;
        }

        case 6:
            setColor(COLOR_CYAN);
            bank.laporanKeuangan();
            resetColor();
            break;

        case 7: {
            string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
            bank.tampilkanDataNasabah(rekening);
            break;
        }

        case 8:
            setColor(COLOR_YELLOW);
            bank.prosesAkhirBulan();
            resetColor();
            break;

        case 9: {
            string rekening = inputTeks("\nNomor Rekening atau ID yang dinonaktifkan : ");
            if (bank.nonaktifkanRekening(rekening)) {
                printColored("Rekening berhasil dinonaktifkan.", COLOR_GREEN);
            } else {
                printColored("Gagal menonaktifkan rekening.", COLOR_RED);
            }
            break;
        }

        case 10:
            bank.tampilkanTopSaldo();
            break;

        case 11: {
            string rekening = inputTeks("\nNomor Rekening atau ID yang dihapus : ");
            if (bank.hapusNasabah(rekening)) {
                printColored("Data nasabah berhasil dihapus.", COLOR_GREEN);
            } else {
                printColored("Data nasabah tidak ditemukan.", COLOR_RED);
            }
            break;
        }

        case 12:
            printColored("\nTerima kasih telah menggunakan Ocean Bank", COLOR_CYAN);
            break;

        default:
            printColored("\nMenu tidak tersedia!", COLOR_RED);
            break;
        }

    } while (pilihan != 12);

    return 0;
}
