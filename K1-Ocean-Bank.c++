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
#include <cmath>      // math helpers (fmod)
#include <algorithm>  // Fitur sort, remove_if, min, dan utilitas algoritma
#include <limits>     // Batas maksimal stream untuk membersihkan input
#include <windows.h>  // Memberi warna pada teks output terminal Windows
#include <set>        // set untuk kontrol akses menu
#include <ctime>      
#include <cctype>     // karakter class checks (isalpha, isdigit)
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

// Aligned label/value printer: ensures colons line up across outputs
const int LABEL_WIDTH = 16;

void printLabel(const string &label, WORD labelColor = COLOR_CYAN) {
    setColor(labelColor);
    cout << setw(LABEL_WIDTH) << left << label << " : ";
    resetColor();
}

void printLabelValue(const string &label, const string &value, WORD labelColor = COLOR_CYAN, WORD valueColor = COLOR_WHITE) {
    printLabel(label, labelColor);
    setColor(valueColor);
    cout << value << endl;
    resetColor();
}

void printLabelValue(const string &label, double value, WORD labelColor = COLOR_CYAN, WORD valueColor = COLOR_GREEN) {
    printLabel(label, labelColor);
    setColor(valueColor);
    cout << fixed << setprecision(2) << value << endl;
    resetColor();
}

void printLabelValue(const string &label, int value, WORD labelColor = COLOR_CYAN, WORD valueColor = COLOR_WHITE) {
    printLabel(label, labelColor);
    setColor(valueColor);
    cout << value << endl;
    resetColor();
}

// Business constants for limits
constexpr double MIN_INITIAL_BALANCE = 10000.0;        // minimal starting balance for new account
constexpr double MAX_INITIAL_BALANCE = 50000000.0;     // maximal starting balance
constexpr double MAX_SETOR_TRANSFER = 50000000.0;      // maximal amount allowed for a single deposit/transfer

vector<string> split(const string &teks, char pemisah) {
    vector<string> bagian;
    string token;
    stringstream ss(teks);

    while (getline(ss, token, pemisah)) {
        bagian.push_back(token);
    }

    return bagian;
}

// Forward declarations for validation helpers used by classes defined earlier
bool isLettersOnly(const string &s);

// ================= ABSTRACT CLASS =================
// Concepts: Class, Abstraction, Polymorphism (via virtual functions)
// === Class (Abstraction) ===
// `BankAccount` is an abstract base class: defines the common interface (abstraction)
// and declares pure virtual methods (polymorphic behavior) for derived account types.
class BankAccount {
protected:
    string nama;
    string spesies;
    string rekening;
    double saldo;
    bool aktif;

public:
    // Constructor: initializes object state
    BankAccount(string nama, string spesies, string rekening, double saldo)
        : nama(nama), spesies(spesies), rekening(rekening), saldo(saldo), aktif(true) {}
    // Constructor: initializes object state

    // Virtual Destructor: ensures proper cleanup when deleting derived objects
    virtual ~BankAccount() {}
    // Virtual Destructor: ensures proper cleanup in derived classes (polymorphism)

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

    void aktifkan() {
        aktif = true;
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
// Concepts: Inheritance (from BankAccount), Encapsulation (private members),
//           Constructor, Destructor, Polymorphism (overrides virtual methods)
class Nasabah : public BankAccount {
private:
    int id;
    double biayaAdminBulanan;

public:
        // Constructor (Nasabah)
        Nasabah(int id, string nama, string spesies, string rekening, double saldo,
                        double biayaAdminBulanan = 2500.0)
                : BankAccount(nama, spesies, rekening, saldo),
                    id(id),
                    biayaAdminBulanan(biayaAdminBulanan) {}

        // Destructor (Nasabah)
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
        resetColor();
        printLabelValue("ID", id, COLOR_CYAN, COLOR_WHITE);
        printLabelValue("Jenis Akun", getJenisAkun(), COLOR_CYAN, COLOR_YELLOW);
        printLabelValue("Nama", nama, COLOR_CYAN, COLOR_WHITE);
        printLabelValue("Spesies", spesies, COLOR_CYAN, COLOR_WHITE);
        printLabelValue("No Rekening", rekening, COLOR_CYAN, COLOR_WHITE);
        printLabelValue("Status", (aktif ? string("AKTIF") : string("NONAKTIF")), COLOR_CYAN, (aktif ? COLOR_GREEN : COLOR_RED));
        printLabelValue("Saldo", saldo, COLOR_CYAN, COLOR_GREEN);
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

    // === Overloading Operator ===
    // Operator `==` overloaded to compare account number string with Nasabah
    bool operator==(const string &noRek) const {
        return rekening == noRek;
    }

    bool operator<(const Nasabah &lain) const {
        return saldo < lain.saldo;
    }

    // Friend operator overload for streaming Nasabah info
    friend ostream &operator<<(ostream &os, const Nasabah &n) {
        os << "[" << n.getJenisAkun() << "] "
           << "ID " << n.getId() << " - "
           << n.getNama() << " (" << n.getRekening() << ")"
           << " Saldo: " << fixed << setprecision(2) << n.getSaldo();
        return os;
    }
};

// ================= CLASS PREMIUM =================
// Concepts: Inheritance, Polymorphism (overrides behavior from Nasabah)
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
        resetColor();
        printLabelValue("ID", getId(), COLOR_BLUE, COLOR_WHITE);
        printLabelValue("Jenis Akun", getJenisAkun(), COLOR_BLUE, COLOR_YELLOW);
        printLabelValue("Nama", getNama(), COLOR_BLUE, COLOR_WHITE);
        printLabelValue("Spesies", getSpesies(), COLOR_BLUE, COLOR_WHITE);
        printLabelValue("No Rekening", getRekening(), COLOR_BLUE, COLOR_WHITE);
        printLabelValue("Status", (isAktif() ? string("AKTIF") : string("NONAKTIF")), COLOR_BLUE, (isAktif() ? COLOR_GREEN : COLOR_RED));
        printLabelValue("Saldo", getSaldo(), COLOR_BLUE, COLOR_GREEN);
        printLabelValue("Bonus Setoran", to_string(bonusSetoranRate * 100) + "%", COLOR_BLUE, COLOR_WHITE);
        setColor(COLOR_BLUE);
        cout << "====================================" << endl;
        resetColor();
    }
};

// ================= CLASS BANK SYSTEM =================
// Concepts: Class, Encapsulation (manages private vector of accounts),
//           File Handling (load/save nasabah.txt), and business logic.
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

    // Returns true if account exists and is active
    bool rekeningAktif(const string &rekening) const {
        const BankAccount *akun = cariByRekening(rekening);
        return akun != nullptr && akun->isAktif();
    }

    void muatDariFile() {
        // === File Handling (read) ===
        // Support three files: combined original, nasabah_reguler.txt, nasabah_premium.txt
        string dir = filePath;
        size_t pos = dir.find_last_of("\\/");
        if (pos != string::npos) dir = dir.substr(0, pos + 1);

        vector<string> filesToLoad = {filePath, dir + string("nasabah_reguler.txt"), dir + string("nasabah_premium.txt")};

        string baris;
        int maxId = 0;

        for (const auto &fp : filesToLoad) {
            ifstream file(fp);
            if (!file.is_open()) continue;

            while (getline(file, baris)) {
                if (baris.empty()) continue;

                vector<string> kolom = split(baris, '|');
                if (kolom.size() < 5) continue;

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

                // If file is explicitly premium/reguler, prefer that
                if (fp.find("premium") != string::npos) jenis = "PREMIUM";
                if (fp.find("reguler") != string::npos) jenis = "REGULER";

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

                if (rekeningSudahAda(rekening)) continue;

                if (jenis == "PREMIUM") {
                    daftarNasabah.push_back(
                        make_unique<PremiumNasabah>(id, nama, spesies, rekening, saldo));
                } else {
                    daftarNasabah.push_back(
                        make_unique<Nasabah>(id, nama, spesies, rekening, saldo));
                }

                if (!aktif) daftarNasabah.back()->nonaktifkan();

                maxId = max(maxId, id);
            }
        }

        idCounter = maxId + 1;
    }

    void simpanSemuaKeFile() const {
        // === File Handling (write) ===
        // Write combined file, and separate reguler / premium files
        string dir = filePath;
        size_t pos = dir.find_last_of("\\/");
        if (pos != string::npos) dir = dir.substr(0, pos + 1);

        string regulerPath = dir + string("nasabah_reguler.txt");
        string premiumPath = dir + string("nasabah_premium.txt");

        ofstream combined(filePath, ios::trunc);
        ofstream regF(regulerPath, ios::trunc);
        ofstream preF(premiumPath, ios::trunc);

        for (const auto &n : daftarNasabah) {
            combined << n->serialize() << endl;
            // Decide which file to write to by dynamic type or akun->getJenisAkun
            if (n->getJenisAkun() == string("PREMIUM")) {
                preF << n->serialize() << endl;
            } else {
                regF << n->serialize() << endl;
            }
        }

        combined.close();
        regF.close();
        preF.close();
    }

    bool tambahNasabah(const string &nama, const string &spesies,
                      const string &rekening, double saldoAwal,
                      bool premium) {
        if (rekeningSudahAda(rekening) || saldoAwal < 0) {
            return false;
        }

        // Validate name: disallow digits or special characters
        if (!isLettersOnly(nama)) {
            printColored("Nama nasabah tidak boleh mengandung angka atau karakter khusus.", COLOR_RED);
            return false;
        }

        // enforce sensible initial balance limits
        if (saldoAwal < MIN_INITIAL_BALANCE || saldoAwal > MAX_INITIAL_BALANCE) {
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

        // Minimum and maximum setor amount rules
        const double MIN_SETOR_TRANSFER = 10000.0;
        if (jumlah < MIN_SETOR_TRANSFER) {
            printColored("Minimal setor adalah 10000.", COLOR_YELLOW);
            return false;
        }
        if (jumlah > MAX_SETOR_TRANSFER) {
            printColored("Maksimal setor per transaksi adalah 50000000.", COLOR_YELLOW);
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

        // Minimum tarik rules: minimum 50000 and must be multiple of 50000
        const double MIN_TARIK = 50000.0;
        if (jumlah < MIN_TARIK) {
            printColored("Minimal tarik adalah 50000 dan harus kelipatan 50000.", COLOR_YELLOW);
            return false;
        }
        if (fmod(jumlah, MIN_TARIK) != 0.0) {
            printColored("Jumlah tarik harus kelipatan 50000.", COLOR_YELLOW);
            return false;
        }

        // If current balance is below 100000, withdrawals are not allowed
        if (akun->getSaldo() < 100000.0) {
            printColored("Saldo kurang dari 100000; tarik tidak diperbolehkan. Anda hanya dapat melakukan transfer jika saldo >= 50000.", COLOR_YELLOW);
            return false;
        }

        // Also ensure requested amount does not exceed balance
        if (jumlah > akun->getSaldo()) {
            printColored("Saldo tidak mencukupi untuk tarik.", COLOR_RED);
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

        // Minimum transfer amount
        const double MIN_SETOR_TRANSFER = 10000.0;
        if (jumlah < MIN_SETOR_TRANSFER) {
            printColored("Minimal transfer adalah 10000.", COLOR_YELLOW);
            return false;
        }

        // If account balance is below 50000, cannot transfer
        if (asal->getSaldo() < 50000.0) {
            printColored("Saldo kurang dari 50000; transfer tidak diperbolehkan.", COLOR_YELLOW);
            return false;
        }

        // Ensure after transfer + fee, balance does not drop below 50000
        if (asal->getSaldo() - (jumlah + biayaTransfer) < 50000.0) {
            printColored("Transfer gagal: saldo setelah transfer tidak boleh kurang dari 50000.", COLOR_YELLOW);
            return false;
        }

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

    // return codes: 0 = not found, 1 = success, 2 = already in requested state
    int nonaktifkanRekening(const string &rekening) {
        BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            return 0; // not found
        }

        if (!akun->isAktif()) {
            return 2; // already non-active
        }

        akun->nonaktifkan();
        simpanSemuaKeFile();
        return 1; // success
    }

    int aktifkanRekening(const string &rekening) {
        BankAccount *akun = cariByRekening(rekening);
        if (akun == nullptr) {
            return 0; // not found
        }

        if (akun->isAktif()) {
            return 2; // already active
        }

        akun->aktifkan();
        simpanSemuaKeFile();
        return 1; // success
    }

    // Helpers to get display info for receipts
    string getNamaNasabah(const string &rekening) const {
        const BankAccount *akun = cariByRekening(rekening);
        return akun ? akun->getNama() : string();
    }

    double getSaldoNasabah(const string &rekening) const {
        const BankAccount *akun = cariByRekening(rekening);
        return akun ? akun->getSaldo() : -1.0;
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
        printLabelValue("Saldo saat ini", akun->getSaldo());
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

        printLabelValue("Total Nasabah", static_cast<int>(daftarNasabah.size()));
        printLabelValue("Total Nasabah Aktif", totalAktif);
        printLabelValue("Total Dana Tersimpan", totalSaldo);
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
        printLabelValue("Total bunga dibagikan", totalBunga);
        printLabelValue("Total admin dipotong", totalAdmin);
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
            return 13;
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

// ================= VALIDATION HELPERS =================
bool isLettersOnly(const string &s) {
    if (s.empty()) return false;
    for (char ch : s) {
        unsigned char c = static_cast<unsigned char>(ch);
        if (!(isalpha(c) || isspace(c))) return false;
    }
    return true;
}

bool isSpeciesValid(const string &s) {
    if (s.empty()) return false;
    for (char ch : s) {
        unsigned char c = static_cast<unsigned char>(ch);
        if (!(isalpha(c) || isspace(c) || ch == '-' || ch == '\'')) return false;
    }
    return true;
}

bool isDigitsOnly(const string &s) {
    if (s.empty()) return false;
    return all_of(s.begin(), s.end(), [](char ch){ return isdigit(static_cast<unsigned char>(ch)); });
}

// ================= FUNCTION =================
void tampilMenuAplikasi(bool isAdmin) {
    setColor(COLOR_CYAN);
    cout << "\n========== OCEAN BANK ==========" << endl;
    setColor(COLOR_WHITE);
    if (isAdmin) {
        cout << "1. Input Data Nasabah" << endl;
        cout << "2. Laporan Keuangan" << endl;
        cout << "3. Cek Data Nasabah" << endl;
        cout << "4. Proses Akhir Bulan (Bunga + Admin)" << endl;
        cout << "5. Nonaktifkan Rekening" << endl;
        cout << "6. Aktifkan Rekening" << endl;
        cout << "7. Tampilkan Top Saldo" << endl;
    } else {
        cout << "1. Cek Saldo" << endl;
        cout << "2. Setor Saldo" << endl;
        cout << "3. Tarik Saldo" << endl;
        cout << "4. Transfer Antar Rekening" << endl;
    }
    setColor(COLOR_YELLOW);
    if (isAdmin) cout << "8. Keluar" << endl; else cout << "5. Keluar" << endl;
    setColor(COLOR_CYAN);
    cout << "Pilih Menu : ";
    resetColor();
}

void tampilMenuUtama() {
    setColor(COLOR_CYAN);
    cout << "\n========== OCEAN BANK - MAIN ==========" << endl;
    setColor(COLOR_WHITE);
    cout << "1. Login Admin" << endl;
    cout << "2. Login Nasabah" << endl;
    setColor(COLOR_YELLOW);
    cout << "3. Keluar" << endl;
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

    // Object instantiation: create BankSystem object that manages accounts
    BankSystem bank(lokasiData);

    int utama = 0;
    do {
        tampilMenuUtama();
        utama = inputAngka("");

        switch (utama) {

        case 1:
        case 2: {
            int pilihan = 0;
            bool isAdmin = false;
            string currentRek;

            // Authentication step: note menu numbering changed (1=Admin, 2=Nasabah)
            if (utama == 2) {
                // Nasabah login: ask rekening once and a fixed password
                string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
                if (!bank.rekeningSudahAda(rekening)) {
                    printColored("Data nasabah tidak ditemukan.", COLOR_RED);
                    break;
                }
                if (!bank.rekeningAktif(rekening)) {
                    printColored("Rekening dinonaktifkan. Tidak dapat login.", COLOR_RED);
                    break;
                }
                string pw = inputTeks("Password Nasabah: ");
                if (pw != "123456") {
                    printColored("Password salah.", COLOR_RED);
                    break;
                }
                currentRek = rekening;
                isAdmin = false;
            } else {
                // Admin login: ask for admin password
                string pw = inputTeks("\nPassword Admin: ");
                if (pw != "789010") {
                    printColored("Password admin salah.", COLOR_RED);
                    break;
                }
                isAdmin = true;
            }

            int exitOption = isAdmin ? 8 : 5;

            do {
                tampilMenuAplikasi(isAdmin);
                pilihan = inputAngka("");
                if (isAdmin) {
                    switch (pilihan) {
                    case 1: { // Input Data Nasabah
                        string nama;
                        do {
                            nama = inputTeks("\nNama Nasabah      : ");
                            if (!isLettersOnly(nama)) {
                                printColored("Nama harus berisi huruf dan spasi saja.", COLOR_RED);
                            }
                        } while (!isLettersOnly(nama));

                        string spesies;
                        do {
                            spesies = inputTeks("Spesies Laut      : ");
                            if (!isSpeciesValid(spesies)) {
                                printColored("Spesies hanya boleh huruf, spasi, '-' atau '\''.", COLOR_RED);
                            }
                        } while (!isSpeciesValid(spesies));

                        string rekening;
                        do {
                            rekening = inputTeks("Nomor Rekening    : ");
                            if (!isDigitsOnly(rekening)) {
                                printColored("Nomor rekening harus angka saja.", COLOR_RED);
                                continue;
                            }
                            if (bank.rekeningSudahAda(rekening)) {
                                printColored("Nomor rekening sudah terdaftar!", COLOR_RED);
                                rekening.clear();
                                continue;
                            }
                            break;
                        } while (true);

                        double saldoAwal;
                        do {
                            saldoAwal = inputNominal("Saldo Awal        : ");
                            if (saldoAwal < MIN_INITIAL_BALANCE) {
                                printColored("Saldo awal minimal adalah 10000.", COLOR_RED);
                                continue;
                            }
                            if (saldoAwal > MAX_INITIAL_BALANCE) {
                                printColored("Saldo awal maksimal adalah 50000000.", COLOR_RED);
                                continue;
                            }
                            break;
                        } while (true);

                        int jenis;
                        do {
                            jenis = inputAngka("Jenis Akun (1 Reguler, 2 Premium): ");
                            if (jenis != 1 && jenis != 2) {
                                printColored("Jenis akun harus 1 (Reguler) atau 2 (Premium).", COLOR_RED);
                            }
                        } while (jenis != 1 && jenis != 2);

                        bool premium = (jenis == 2);
                        if (bank.tambahNasabah(nama, spesies, rekening, saldoAwal, premium)) {
                            printColored("Nasabah berhasil ditambahkan.", COLOR_GREEN);
                        } else {
                            printColored("Gagal menambahkan nasabah.", COLOR_RED);
                        }
                        break;
                    }

                    case 2: // Laporan Keuangan
                        setColor(COLOR_CYAN);
                        bank.laporanKeuangan();
                        resetColor();
                        break;

                    case 3: { // Cek Data Nasabah
                        string rekening = inputTeks("\nMasukkan Nomor Rekening atau ID : ");
                        bank.tampilkanDataNasabah(rekening);
                        break;
                    }

                    case 4: // Proses Akhir Bulan
                        setColor(COLOR_YELLOW);
                        bank.prosesAkhirBulan();
                        resetColor();
                        break;

                    case 5: { // Nonaktifkan
                        string rekening = inputTeks("\nNomor Rekening atau ID yang dinonaktifkan : ");
                        int status = bank.nonaktifkanRekening(rekening);
                        if (status == 1) {
                            printColored("Rekening berhasil dinonaktifkan.", COLOR_GREEN);
                        } else if (status == 2) {
                            printColored("Rekening sudah nonaktif.", COLOR_YELLOW);
                        } else {
                            printColored("Data nasabah tidak ditemukan.", COLOR_RED);
                        }
                        break;
                    }

                    case 6: { // Aktifkan
                        string rekening = inputTeks("\nNomor Rekening atau ID yang diaktifkan : ");
                        int status = bank.aktifkanRekening(rekening);
                        if (status == 1) {
                            printColored("Rekening berhasil diaktifkan.", COLOR_GREEN);
                        } else if (status == 2) {
                            printColored("Rekening sudah aktif.", COLOR_YELLOW);
                        } else {
                            printColored("Data nasabah tidak ditemukan.", COLOR_RED);
                        }
                        break;
                    }

                    case 7: { // Tampilkan Top Saldo (admin)
                        bank.tampilkanTopSaldo();
                        break;
                    }

                    case 8:
                        printColored("\nKeluar dari aplikasi.", COLOR_CYAN);
                        break;

                    default:
                        printColored("\nMenu tidak tersedia!", COLOR_RED);
                        break;
                    }
                } else {
                    switch (pilihan) {
                    case 1: { // Cek Saldo (use logged-in account)
                        bank.cekSaldo(currentRek);
                        break;
                    }

                    case 2: { // Setor + kwitansi (use logged-in account)
                        double jumlah = inputNominal("Jumlah Setor            : ");

                        if (bank.setor(currentRek, jumlah)) {
                            printColored("Setor saldo berhasil.", COLOR_GREEN);
                            // create receipt
                            string dataDir = lokasiData.substr(0, lokasiData.find_last_of("\\/") + 1);
                            // ensure directory exists externally; skip create for portability
                            time_t now = time(nullptr);
                            tm tmnow;
#ifdef _WIN32
                            localtime_s(&tmnow, &now);
#else
                            tmnow = *localtime(&now);
#endif
                            char buf[64];
                            strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmnow);
                            string fname = dataDir + string("kwitansi_setor_") + buf + ".txt";
                            ofstream fout(fname);
                            fout << "KWITANSI SETOR" << endl;
                            fout << "Tanggal     : " << buf << endl;
                            fout << "Rekening    : " << currentRek << endl;
                            fout << "Nama        : " << bank.getNamaNasabah(currentRek) << endl;
                            fout << fixed << setprecision(2);
                            fout << "Jumlah      : " << jumlah << endl;
                            fout << "Saldo Akhir : " << bank.getSaldoNasabah(currentRek) << endl;
                            fout.close();
                            cout << "Kwitansi tersimpan: " << fname << endl;
                        } else {
                            printColored("Setor gagal. Cek data rekening atau nominal.", COLOR_RED);
                        }
                        break;
                    }

                    case 3: { // Tarik + kwitansi (use logged-in account)
                        double jumlah = inputNominal("Jumlah Tarik            : ");

                        if (bank.tarik(currentRek, jumlah)) {
                            printColored("Tarik saldo berhasil.", COLOR_GREEN);
                            string dataDir = lokasiData.substr(0, lokasiData.find_last_of("\\/") + 1);
                            // ensure directory exists externally; skip create for portability
                            time_t now = time(nullptr);
                            tm tmnow;
#ifdef _WIN32
                            localtime_s(&tmnow, &now);
#else
                            tmnow = *localtime(&now);
#endif
                            char buf[64];
                            strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmnow);
                            string fname = dataDir + string("kwitansi_tarik_") + buf + ".txt";
                            ofstream fout(fname);
                            fout << "KWITANSI TARIK" << endl;
                            fout << "Tanggal     : " << buf << endl;
                            fout << "Rekening    : " << currentRek << endl;
                            fout << "Nama        : " << bank.getNamaNasabah(currentRek) << endl;
                            fout << fixed << setprecision(2);
                            fout << "Jumlah      : " << jumlah << endl;
                            fout << "Saldo Akhir : " << bank.getSaldoNasabah(currentRek) << endl;
                            fout.close();
                            cout << "Kwitansi tersimpan: " << fname << endl;
                        } else {
                            printColored("Tarik gagal. Saldo tidak cukup / rekening tidak valid.", COLOR_RED);
                        }
                        break;
                    }

                    case 4: { // Transfer + kwitansi (use logged-in account as source)
                        string tujuan = inputTeks("Rekening Tujuan (nomor atau ID): ");
                        double jumlah = inputNominal("Nominal Transfer: ");

                        if (bank.transfer(currentRek, tujuan, jumlah)) {
                            printColored("Transfer berhasil (biaya transfer 2500).", COLOR_GREEN);
                            string dataDir = lokasiData.substr(0, lokasiData.find_last_of("\\/") + 1);
                            // ensure directory exists externally; skip create for portability
                            time_t now = time(nullptr);
                            tm tmnow;
#ifdef _WIN32
                            localtime_s(&tmnow, &now);
#else
                            tmnow = *localtime(&now);
#endif
                            char buf[64];
                            strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmnow);
                            string fname = dataDir + string("kwitansi_transfer_") + buf + ".txt";
                            ofstream fout(fname);
                            fout << "KWITANSI TRANSFER" << endl;
                            fout << "Tanggal       : " << buf << endl;
                            fout << "Rekening Asal : " << currentRek << endl;
                            fout << "Nama Asal     : " << bank.getNamaNasabah(currentRek) << endl;
                            fout << "Rekening Tujuan: " << tujuan << endl;
                            fout << "Nama Tujuan   : " << bank.getNamaNasabah(tujuan) << endl;
                            fout << fixed << setprecision(2);
                            fout << "Jumlah        : " << jumlah << endl;
                            fout << "Biaya Transfer: 2500.00" << endl;
                            fout << "Saldo Akhir Asal   : " << bank.getSaldoNasabah(currentRek) << endl;
                            fout << "Saldo Akhir Tujuan : " << bank.getSaldoNasabah(tujuan) << endl;
                            fout.close();
                            cout << "Kwitansi tersimpan: " << fname << endl;
                        } else {
                            printColored("Transfer gagal. Cek rekening atau saldo.", COLOR_RED);
                        }
                        break;
                    }

                    case 5:
                        printColored("\nKeluar dari aplikasi.", COLOR_CYAN);
                        break;

                    default:
                        printColored("\nMenu tidak tersedia!", COLOR_RED);
                        break;
                    }
                }

            } while (pilihan != exitOption);

            break;
        }

            

        case 3:
            printColored("\nTerima kasih telah menggunakan Ocean Bank", COLOR_CYAN);
            break;

        default:
            printColored("\nMenu tidak tersedia!", COLOR_RED);
            break;
        }

    } while (utama != 3);

    return 0;
}
