// Kelompok 1 - Praktikum Pemrograman Berorientasi Objek
// 1. Khairunnisa - 251402017
// 2. Muhammad Ihsan Anwar - 251402044
// 3. Nadia Stevany Br Situmorang - 251402073
// 4. Viter Moldy Kesuma - 251402079
// 5. Agnes Natalia Br Siregar - 251402108
// 6. Limjun Basani Sipayung - 251402144

// SISTEM OCEAN BANK

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;

// ================= ABSTRACT CLASS =================
class BankAccount {
protected:
    string nama;
    string spesies;
    string rekening;
    double saldo;

public:
    virtual void tampilkanData() = 0;
    virtual void setor(double jumlah) = 0;
    virtual void tarik(double jumlah) = 0;
    virtual ~BankAccount() {}
};

// ================= CLASS NASABAH =================
class Nasabah : public BankAccount {
private:
    int id;

public:
    // Constructor
    Nasabah(int id, string nama, string spesies,
             string rekening, double saldo) {
        this->id = id;
        this->nama = nama;
        this->spesies = spesies;
        this->rekening = rekening;
        this->saldo = saldo;
    }

    // Destructor
    ~Nasabah() {
        // Destructor
    }

    // Getter
    int getId() {
        return id;
    }

    string getNama() {
        return nama;
    }

    string getSpesies() {
        return spesies;
    }

    string getRekening() {
        return rekening;
    }

    double getSaldo() {
        return saldo;
    }

    // Setter
    void setSaldo(double saldoBaru) {
        saldo = saldoBaru;
    }

    // Overriding Polymorphism
    void tampilkanData() override {
        cout << "====================================" << endl;
        cout << "ID           : " << id << endl;
        cout << "Nama         : " << nama << endl;
        cout << "Spesies      : " << spesies << endl;
        cout << "No Rekening  : " << rekening << endl;
        cout << "Saldo        : " << saldo << endl;
        cout << "====================================" << endl;
    }

    void setor(double jumlah) override {
        saldo += jumlah;
        cout << "Setor saldo berhasil!" << endl;
    }

    void tarik(double jumlah) override {
        if (jumlah > saldo) {
            cout << "Saldo tidak mencukupi!" << endl;
        } else {
            saldo -= jumlah;
            cout << "Tarik saldo berhasil!" << endl;
        }
    }

    // Operator Overloading
    bool operator==(string noRek) {
        return rekening == noRek;
    }

    // Simpan ke file
    void simpanFile() {
        ofstream file("nasabah.txt", ios::app);

        file << id << "|"
             << nama << "|"
             << spesies << "|"
             << rekening << "|"
             << saldo << endl;

        file.close();
    }
};

// ================= CLASS PREMIUM =================
class PremiumNasabah : public Nasabah {
private:
    double bonus;

public:
    PremiumNasabah(int id, string nama, string spesies,
                   string rekening, double saldo, double bonus)
        : Nasabah(id, nama, spesies, rekening, saldo) {
        this->bonus = bonus;
    }

    void tampilkanBonus() {
        cout << "Bonus Premium : " << bonus << endl;
    }
};

// ================= FUNCTION =================
void tampilMenu() {
    cout << "\n========== OCEAN BANK ==========" << endl;
    cout << "1. Input Data Nasabah" << endl;
    cout << "2. Cek Saldo" << endl;
    cout << "3. Setor Saldo" << endl;
    cout << "4. Tarik Saldo" << endl;
    cout << "5. Laporan Keuangan" << endl;
    cout << "6. Cek Data Nasabah" << endl;
    cout << "7. Keluar" << endl;
    cout << "Pilih Menu : ";
}

// ================= MAIN =================
int main() {
    vector<Nasabah> daftarNasabah;

    int pilihan;
    int idCounter = 1;

    do {
        tampilMenu();
        cin >> pilihan;

        switch (pilihan) {

        // ================= INPUT NASABAH =================
        case 1: {
            string nama, spesies, rekening;
            double saldo;

            cin.ignore();

            cout << "\nNama Nasabah      : ";
            getline(cin, nama);

            cout << "Spesies Laut      : ";
            getline(cin, spesies);

            cout << "Nomor Rekening    : ";
            getline(cin, rekening);

            cout << "Saldo Awal         : ";
            cin >> saldo;

            Nasabah n(idCounter, nama, spesies, rekening, saldo);

            daftarNasabah.push_back(n);
            daftarNasabah.back().simpanFile();

            cout << "\nNasabah berhasil ditambahkan!" << endl;

            idCounter++;
            break;
        }

        // ================= CEK SALDO =================
        case 2: {
            string rekening;
            bool ditemukan = false;

            cin.ignore();

            cout << "\nMasukkan Nomor Rekening : ";
            getline(cin, rekening);

            for (auto &n : daftarNasabah) {
                if (n == rekening) {
                    cout << "Saldo Saat Ini : " << n.getSaldo() << endl;
                    ditemukan = true;
                }
            }

            if (!ditemukan) {
                cout << "Data tidak ditemukan!" << endl;
            }

            break;
        }

        // ================= SETOR =================
        case 3: {
            string rekening;
            double jumlah;
            bool ditemukan = false;

            cin.ignore();

            cout << "\nMasukkan Nomor Rekening : ";
            getline(cin, rekening);

            cout << "Jumlah Setor : ";
            cin >> jumlah;

            for (auto &n : daftarNasabah) {
                if (n == rekening) {
                    n.setor(jumlah);
                    ditemukan = true;
                }
            }

            if (!ditemukan) {
                cout << "Data tidak ditemukan!" << endl;
            }

            break;
        }

        // ================= TARIK =================
        case 4: {
            string rekening;
            double jumlah;
            bool ditemukan = false;

            cin.ignore();

            cout << "\nMasukkan Nomor Rekening : ";
            getline(cin, rekening);

            cout << "Jumlah Tarik : ";
            cin >> jumlah;

            for (auto &n : daftarNasabah) {
                if (n == rekening) {
                    n.tarik(jumlah);
                    ditemukan = true;
                }
            }

            if (!ditemukan) {
                cout << "Data tidak ditemukan!" << endl;
            }

            break;
        }

        // ================= LAPORAN =================
        case 5: {
            double totalSaldo = 0;

            cout << "\n========== LAPORAN KEUANGAN ==========" << endl;

            for (auto &n : daftarNasabah) {
                n.tampilkanData();
                totalSaldo += n.getSaldo();
            }

            cout << "Total Nasabah : "
                 << daftarNasabah.size() << endl;

            cout << "Total Saldo   : "
                 << totalSaldo << endl;

            break;
        }

        // ================= CEK DATA =================
        case 6: {
            string rekening;
            bool ditemukan = false;

            cin.ignore();

            cout << "\nMasukkan Nomor Rekening : ";
            getline(cin, rekening);

            for (auto &n : daftarNasabah) {
                if (n == rekening) {
                    n.tampilkanData();
                    ditemukan = true;
                }
            }

            if (!ditemukan) {
                cout << "Data tidak ditemukan!" << endl;
            }

            break;
        }

        case 7:
            cout << "\nTerima kasih telah menggunakan Ocean Bank" << endl;
            break;

        default:
            cout << "\nMenu tidak tersedia!" << endl;
        }

    } while (pilihan != 7);

    return 0;
}
