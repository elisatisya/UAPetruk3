#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stack>
#include <queue>
#include <map>
#include <fstream>
#include <windows.h>
#include <cmath>

using namespace std;

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct Pengeluaran {
    string tanggal;
    string keterangan;
    string kategori;
    double jumlah;
};

bool bandingTanggal(const string& t1, const string& t2) {
    int d1 = stoi(t1.substr(0, 2));
    int m1 = stoi(t1.substr(3, 2));
    int y1 = stoi(t1.substr(6, 4));

    int d2 = stoi(t2.substr(0, 2));
    int m2 = stoi(t2.substr(3, 2));
    int y2 = stoi(t2.substr(6, 4));

    if (y1 != y2) return y1 < y2;
    if (m1 != m2) return m1 < m2;
    return d1 < d2;
}

int cariTanggal(const vector<Pengeluaran>& data, const string& target, bool cariAwal) {
    int kiri = 0, kanan = data.size() - 1, hasil = -1;
    while (kiri <= kanan) {
        int tengah = (kiri + kanan) / 2;
        if ((cariAwal && !bandingTanggal(data[tengah].tanggal, target)) ||
            (!cariAwal && !bandingTanggal(target, data[tengah].tanggal))) {
            hasil = tengah;
            if (cariAwal) kanan = tengah - 1;
            else kiri = tengah + 1;
        } else {
            if (cariAwal) kiri = tengah + 1;
            else kanan = tengah - 1;
        }
    }
    return hasil;
}

bool cekLogin(const string& username, const string& password) {
    ifstream file("akun.txt");
    string line;


    while (getline(file, line)) {
        size_t delimiter = line.find(';');
        if (delimiter != string::npos) {
            string user = line.substr(0, delimiter);
            string pass = line.substr(delimiter + 1);
            if (user == username && pass == password) {
                return true;
            }
        }
    }
    return false;
}

void registrasiAkun() {
    string username, password;

    system("cls");

    gotoxy(45,9);
    cout << "==== Registrasi Akun ====";
    gotoxy(50,10);
    cout << "Buat username: ";
    cin >> username;
    gotoxy(50,11);
    cout << "Buat password: ";
    cin >> password;

    ofstream file("akun.txt", ios::app);
    file << username << ";" << password << endl;
    file.close();

    gotoxy(40,12);
    cout << "Akun berhasil dibuat! Silakan login.\n";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string prosesLogin() {
    string username, password;

    system("cls");

    gotoxy(50,9);
    cout << "==== Login ====";
    gotoxy(50,10);
    cout << "Username: ";
    cin >> username;
    gotoxy(50,11);
    cout << "Password: ";
    cin >> password;

    if (cekLogin(username, password)) {
        gotoxy(40,12);
        cout << "Login berhasil. Selamat datang, " << username << "!\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return username;
    } else {
        gotoxy(40,12);
        cout << "Login gagal. Username atau password salah.\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
        return "";
    }
}

class KosFinance {
private:
    vector<Pengeluaran> daftarPengeluaran;
    stack<Pengeluaran> undoStack;
    queue<Pengeluaran> antreanKonfirmasi;
    string currentUsername;


    string getFilename() const {
        return "data_" + currentUsername + ".txt";
    }

public:
    KosFinance(const string& username = "") : currentUsername(username) {}

    void setUsername(const string& username) {
        currentUsername = username;
    }
    void simpanDataPengeluaran() const {
        ofstream file(getFilename());
        for (const auto& p : daftarPengeluaran) {
            file << p.tanggal << ';' << p.keterangan << ';' << p.jumlah << ';' << p.kategori << '\n';
        }
    }

    void muatDataPengeluaran() {
        daftarPengeluaran.clear();
        ifstream file(getFilename());
        string line;
        while (getline(file, line)) {
            Pengeluaran p;
            stringstream ss(line);
            getline(ss, p.tanggal, ';');
            getline(ss, p.keterangan, ';');
            string jumlahStr;
            getline(ss, jumlahStr, ';');
            p.jumlah = stod(jumlahStr);
            getline(ss, p.kategori, ';');
            daftarPengeluaran.push_back(p);
        }
    }

    void tambahPengeluaran() {
        Pengeluaran baru;
        system("cls");

        bool vT = false;
        do {
            cout << "Masukkan tanggal, harus angka dan sesuai format (dd/mm/yyyy): " << endl;
            getline(cin, baru.tanggal);
            string &t = baru.tanggal;
            if(t.size()==10 && isdigit(t[0])&&isdigit(t[1])&&t[2]=='/' &&
               isdigit(t[3])&&isdigit(t[4])&&t[5]=='/' &&
               isdigit(t[6])&&isdigit(t[7])&&isdigit(t[8])&&isdigit(t[9]))
                vT = true;
            else cout << "Format tanggal harus dd/mm/yyyy dan berupa angka.\n";
        } while(!vT);

        bool vK = false;
        do {
            cout << "Masukkan keterangan: ";
            getline(cin, baru.keterangan);
            vK = !baru.keterangan.empty() &&
                     all_of(baru.keterangan.begin(), baru.keterangan.end(),
                            [](char c){ return isalpha(c)||isspace(c); });
            if(!vK) cout << "Keterangan harus berupa huruf/spasi (string) dan tidak boleh kosong!\n";
        } while(!vK);

        bool vJ = false;
        do {
            cout << "Masukkan jumlah pengeluaran: ";
            if(cin >> baru.jumlah) {
                if (baru.jumlah > 0) {
                    vJ = true;
                } else {
                    cout << "Jumlah harus lebih besar dari nol!\n";
                }
            }
            else {
                cout << "Jumlah harus berupa angka!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        } while(!vJ);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Pilih kategori:\n"
             << "1. Makanan\n2. Minuman\n3. Transportasi\n4. Belanja\n5. Lainnya\n";
        cout << "Pilihan (1-5): ";
        int kp; cin >> kp;
        switch(kp){
            case 1: baru.kategori="Makanan"; break;
            case 2: baru.kategori="Minuman"; break;
            case 3: baru.kategori="Transportasi"; break;
            case 4: baru.kategori="Belanja"; break;
            case 5: baru.kategori="Lainnya"; break;
            default:
                cout << "Pilihan tidak valid, kategori di-set ke 'Lainnya'.\n";
                baru.kategori = "Lainnya";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        antreanKonfirmasi.push(baru);

        Pengeluaran p = antreanKonfirmasi.front();
        cout << "\nKonfirmasi pengeluaran berikut?\n";
        cout << "Tanggal    : " << p.tanggal << endl;
        cout << "Keterangan : " << p.keterangan << endl;
        cout << "Jumlah     : Rp" << fixed << setprecision(2) << p.jumlah << endl;
        cout << "Kategori   : " << p.kategori << endl;
        cout << "Konfirmasi pengeluaran ini? (y/n): ";
        char pilih;
        cin >> pilih;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (pilih == 'y' || pilih == 'Y') {
            daftarPengeluaran.push_back(p);
            cout << "Pengeluaran berhasil ditambahkan!\n";
            simpanDataPengeluaran();
        } else {
            cout << "Pengeluaran dibatalkan.\n";
        }
        antreanKonfirmasi.pop();
        cin.get();
    }

    void lihatSemuaPengeluaran() const {
        system("cls");

        cout << "=== Daftar Pengeluaran ===\n";
        if (daftarPengeluaran.empty()) {
            cout << "Belum ada data pengeluaran.\n";
        } else {
            cout << "-------------------------------------------------------------------------------\n";
            cout << "| No. | Tanggal    | Keterangan                  |    Jumlah    |  Kategori   |\n";
            cout << "-------------------------------------------------------------------------------\n";
            double total = 0;
            for (size_t i = 0; i < daftarPengeluaran.size(); ++i) {
                const Pengeluaran& p = daftarPengeluaran[i];
                cout << "| " << setw(3) << left << i + 1 << " "
                     << "| " << setw(10) << left << p.tanggal << " "
                     << "| " << setw(28) << left << p.keterangan << " "
                     << "| " << setw(11) << right << fixed << setprecision(2) << p.jumlah << " "
                     << "| " << setw(12) << left << p.kategori << "|\n";
                total += p.jumlah;
            }
            cout << "-------------------------------------------------------------------------------\n";
            cout << " TOTAL PENGELUARAN: Rp " << total << endl;
            cout << "-------------------------------------------------------------------------------\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void hitungTotalRangeTanggal() const {
        system("cls");

        if (daftarPengeluaran.empty()) {
            cout << "Tidak ada data untuk dihitung.\n";
            cin.get();
            return;
        }

        string dari, sampai;
        cout << "Masukkan tanggal awal (dd/mm/yyyy): ";
        getline(cin, dari);
        cout << "Masukkan tanggal akhir (dd/mm/yyyy): ";
        getline(cin, sampai);

        if (dari.length() != 10 || sampai.length() != 10) {
            cout << "Format tanggal harus dd/mm/yyyy.\n";
            cin.get();
            return;
        }

        int indeksAwal = cariTanggal(daftarPengeluaran, dari, true);
        int indeksAkhir = cariTanggal(daftarPengeluaran, sampai, false);

        if (indeksAwal == -1 || indeksAkhir == -1 || indeksAwal > indeksAkhir) {
            cout << "Rentang tanggal tidak valid atau tidak ditemukan.\n";
            cin.get();
            return;
        }

        double total = 0;
        for (int i = indeksAwal; i <= indeksAkhir; ++i) {
            total += daftarPengeluaran[i].jumlah;
        }


        cout << "\nTotal pengeluaran dari " << dari << " sampai " << sampai << " adalah: Rp" << fixed << setprecision(2) << total << endl;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void urutkanPengeluaranHarian() {
        system("cls");

        if (daftarPengeluaran.empty()) {
            cout << "Belum ada data pengeluaran.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }

        vector<Pengeluaran> pengeluaranHarian = daftarPengeluaran;
        sort(pengeluaranHarian.begin(), pengeluaranHarian.end(),
             [](const Pengeluaran& a, const Pengeluaran& b) {
                 return a.jumlah < b.jumlah;
             });
             
        cout << "\n=== Urutan Pengeluaran Harian (Terkecil ke Terbesar) ===\n";
        cout << "--------------------------------------------------------------------------------\n";
        cout << "| No. | Tanggal    | Keterangan                   | Jumlah       | Kategori    |\n";
        cout << "--------------------------------------------------------------------------------\n";

        for (size_t i = 0; i < pengeluaranHarian.size(); ++i) {
            cout << "| " << setw(3) << i + 1 << " ";
            cout << "| " << setw(10) << pengeluaranHarian[i].tanggal << " ";
            cout << "| " << setw(28) << pengeluaranHarian[i].keterangan << " ";
            cout << "| " << setw(12) << fixed << setprecision(2) << pengeluaranHarian[i].jumlah << " ";
            cout << "| " << setw(12) << pengeluaranHarian[i].kategori << " |\n";
        }
        cout << "--------------------------------------------------------------------------------\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void tampilkanPengeluaranTerbesar() {
        system("cls");
        cout << "=== Pengeluaran Terbesar ===\n";
        if (daftarPengeluaran.empty()) {
            cout << "Belum ada data pengeluaran.\n";
        } else {
            auto it = max_element(daftarPengeluaran.begin(), daftarPengeluaran.end(),
                [](const Pengeluaran& a, const Pengeluaran& b) {
                    return a.jumlah < b.jumlah;
                });
            cout << "-------------------------------------------------------------------------------\n";
            cout << "| No. | Tanggal    | Keterangan                   |   Jumlah    |  Kategori   |\n";
            cout << "-------------------------------------------------------------------------------\n";
            cout << "| " << setw(3) << 1 << " "
                 << "| " << setw(10) << left << it->tanggal << " "
                 << "| " << setw(28) << left << it->keterangan << " "
                 << "| " << setw(11) << right << fixed << setprecision(2) << it->jumlah << " "
                 << "| " << setw(12) << left << it->kategori << "|\n";
            cout << "-------------------------------------------------------------------------------\n";
        }
       
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void undoPengeluaranTerakhir() {
        system("cls");

        if (undoStack.empty()) {
            cout << "Tidak ada pengeluaran yang bisa di-undo.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }

        Pengeluaran terakhir = undoStack.top();
        undoStack.pop();

        auto it = find_if(daftarPengeluaran.begin(), daftarPengeluaran.end(),
            [&](const Pengeluaran& p) {
                return p.tanggal == terakhir.tanggal &&
                       p.keterangan == terakhir.keterangan &&
                       abs(p.jumlah - terakhir.jumlah) < 0.01;
            });

        if (it != daftarPengeluaran.end()) {
            daftarPengeluaran.erase(it);
            cout << "Pengeluaran terakhir berhasil di-undo.\n";
        } else {
            cout << "Gagal undo: data tidak ditemukan di daftar utama.\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void tampilkanStatistikKategori() const {
        system("cls");


        if(daftarPengeluaran.empty()){
            cout << "Belum ada data.\n";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            return;
        }
        map<string,double> mp; double tot=0;
        for(auto &p: daftarPengeluaran){
            mp[p.kategori] += p.jumlah;
            tot += p.jumlah;
        }
        cout << "\n=== Statistik per Kategori ===\n";
        for(auto &it: mp){
            double pct = it.second/tot*100;
            cout << it.first << ": Rp" << fixed << setprecision(2) << it.second
                 << " ("<<setprecision(1) << pct << "%)\n";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void hapusPengeluaran() {
        system("cls");
        cout << "=== Hapus Pengeluaran ===\n";
        if (daftarPengeluaran.empty()) {
            cout << "Belum ada data pengeluaran.\n";
        } else {
            cout << "-------------------------------------------------------------------------------\n";
            cout << "| No. | Tanggal    | Keterangan                   |    Jumlah   |   Kategori  |\n";
            cout << "-------------------------------------------------------------------------------\n";
            for (size_t i = 0; i < daftarPengeluaran.size(); ++i) {
                const Pengeluaran& p = daftarPengeluaran[i];
                cout << "| " << setw(3) << right << i + 1 << " "
                     << "| " << setw(10) << left << p.tanggal << " "
                     << "| " << setw(28) << left << p.keterangan << " "
                     << "| " << setw(11) << right << fixed << setprecision(2) << p.jumlah << " "
                     << "| " << setw(12) << left << p.kategori << "|\n";
            }
            cout << "-------------------------------------------------------------------------------\n";
            cout << "Masukkan nomor pengeluaran yang ingin dihapus: ";
            size_t idx;
            cin >> idx;
            if (idx < 1 || idx > daftarPengeluaran.size()) {
                cout << "Nomor tidak valid.\n";
            } else {
                daftarPengeluaran.erase(daftarPengeluaran.begin() + idx - 1);
                cout << "Pengeluaran berhasil dihapus.\n";
                simpanDataPengeluaran();
            }
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
};

void tampilkanMenu() {
    system("cls");
    gotoxy(36, 6);  cout << "=======================================================";
    gotoxy(36, 7);  cout << "| KosFinance: Aplikasi Pengelola Pengeluaran Anak Kos |";
    gotoxy(36, 8);  cout << "=======================================================";
    gotoxy(36, 9);  cout << "| 1. Tambah Pengeluaran                               |";
    gotoxy(36,10);  cout << "| 2. Lihat Semua Pengeluaran                          |";
    gotoxy(36,11);  cout << "| 3. Hapus Pengeluaran                                |";
    gotoxy(36,12);  cout << "| 4. Total Pengeluaran Berdasarkan Rentang Tanggal    |";
    gotoxy(36,13);  cout << "| 5. Urutkan Pengeluaran Harian (Terkecil ke Terbesar)|";
    gotoxy(36,14);  cout << "| 6. Tampilkan Pengeluaran Terbesar                   |";
    gotoxy(36,15);  cout << "| 7. Undo Pengeluaran Terakhir                        |";
    gotoxy(36,16);  cout << "| 8. Tampilkan Statistik Pengeluaran per Kategori     |";
    gotoxy(36,17);  cout << "| 9. Keluar                                           |";
    gotoxy(36,18);  cout << "=======================================================";
    gotoxy(36,19);  cout << "Pilih menu (1-9): ";
}

void tampilkanAwal() {
    system("cls");
    gotoxy(44, 8);  cout << "=======================================";
    gotoxy(44, 9);  cout << "|     Selamat Datang di KosFinance    |";
    gotoxy(44, 10); cout << "=======================================";
    gotoxy(44, 11); cout << "| 1. Login                            |";
    gotoxy(44, 12); cout << "| 2. Registrasi Akun                  |";
    gotoxy(44, 13); cout << "| 3. Keluar                           |";
    gotoxy(44, 14); cout << "=======================================";
    gotoxy(44, 15); cout << "Pilih opsi (1-3): ";
}

int main() {
    int pilihAuth;
    string currentUsername = "";
    KosFinance aplikasi;

    do {
        tampilkanAwal();
        gotoxy(44 + strlen("Pilih opsi (1-3): "), 15);
        cin >> pilihAuth;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (pilihAuth) {
            case 1:
                currentUsername = prosesLogin();
                if (!currentUsername.empty()) {
                    aplikasi.setUsername(currentUsername);
                    aplikasi.muatDataPengeluaran();
                }
                 break;
            case 2:
                registrasiAkun();
                break;
            case 3:
                cout << "Keluar dari aplikasi.\n";
                return 0;
            default:
                cout << "Pilihan tidak valid. Coba lagi.\n";
                cin.get();
        }
    } while (currentUsername.empty());

    int pilihan;
    do {
        tampilkanMenu();
        gotoxy(38 + strlen("Pilih menu (1-9): "), 19);
        cin >> pilihan;
    
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            system("cls");
            cout << "Input tidak valid. Masukkan angka pilihan menu.\n";
            cin.get();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

switch (pilihan) {
            case 1:
                aplikasi.tambahPengeluaran();
                break;
            case 2:
                aplikasi.lihatSemuaPengeluaran();
                break;
            case 3:
                aplikasi.hapusPengeluaran();
                break;
            case 4:
                aplikasi.hitungTotalRangeTanggal();
                break;
            case 5:
                aplikasi.urutkanPengeluaranHarian();
                break;
            case 6:
                aplikasi.tampilkanPengeluaranTerbesar();
                break;
            case 7:
                aplikasi.undoPengeluaranTerakhir();
                break;
            case 8:
                aplikasi.tampilkanStatistikKategori();
                break;
            case 9:
                aplikasi.simpanDataPengeluaran();
                cout << "Terima kasih telah menggunakan KosFinance!\n";
                break;
            default:
                system("cls");
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
                cin.get();
        }


    } while (pilihan != 9);


    return 0;
}
