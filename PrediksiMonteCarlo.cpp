#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <random>
#include <stdio.h>
using namespace std;

// Fungsi untuk membaca data CSV
void readCSV(const string& filename, double*& prices, int& size) {
    ifstream file(filename);
    string line, cell;

    getline(file, line);

    size = 0;
    while (getline(file, line)) {
        size++;
    }

    prices = new double[size];

    // Membaca harga emas dari file CSV
    file.clear();
    file.seekg(0);
    getline(file, line);

    int index = 0;
    while (getline(file, line)) {
        stringstream lineStream(line);
        getline(lineStream, cell, ',');
        getline(lineStream, cell, ',');
        try {
            prices[index++] = stod(cell);
        } catch (...) {
            continue;
        }
    }

    file.close();
}

// Fungsi untuk menghitung log return
void calculateLogReturns(const double* prices, int size, double*& logReturns) {
    logReturns = new double[size - 1];
    for (int i = 1; i < size; ++i) {
        logReturns[i - 1] = log(prices[i] / prices[i - 1]);
    }
}

// Fungsi untuk menghitung rata-rata dan standar deviasi
void calculateStatistics(const double* data, int size, double& mean, double& standarDeviasi) {
    double sum = 0.0, sumSq = 0.0;
    for (int i = 0; i < size; ++i) {
        sum += data[i];
        sumSq += data[i] * data[i];
    }
    mean = sum / size;
    standarDeviasi = sqrt((sumSq / size) - (mean * mean));
    cout << "Rata-rata Log Returns : " << mean << endl;
    cout << "Standar Deviasi       : " << standarDeviasi << endl;
}

// Fungsi untuk simulasi Monte Carlo
void monteCarloSimulation(double initialPrice, double mean, double standarDeviasi, int months, int simulations, double*& simulatedPrices) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dist(0, 1);

    simulatedPrices = new double[simulations];

    for (int sim = 0; sim < simulations; ++sim) {
        double price = initialPrice;
        for (int month = 0; month < months; ++month) {
            double random = dist(gen);
            price *= exp((mean - 0.5 * standarDeviasi * standarDeviasi) + standarDeviasi * random);
        }
        simulatedPrices[sim] = price;
    }
}

// Fungsi utama
int main() {
    string filename = "HargaEmasBulan.csv";
    double* prices = nullptr;
    int size = 0;

    // Membaca file CSV untuk harga emas
    readCSV(filename, prices, size);

    if (prices == nullptr || size == 0) {
        cerr << "Error: No valid price data found in the file." << endl;
        return 1;
    }

    // Hitung log return bulanan
    double* logReturns = nullptr;
    calculateLogReturns(prices, size, logReturns);

    // Hitung rata-rata dan standar deviasi log return
    double mean = 0.0, standarDeviasi = 0.0;
    calculateStatistics(logReturns, size - 1, mean, standarDeviasi);

    // Menampilkan harga emas terbaru
    double latestPrice = prices[size - 1]; // Harga terakhir dalam dataset
    cout << "Harga emas terbaru (bulan terakhir): " << latestPrice << endl;

    // Meminta input dari user untuk jumlah bulan yang ingin diprediksi
    int months;
    cout << "Masukkan jumlah bulan yang ingin diprediksi: ";
    cin >> months;

    // Tetapkan jumlah simulasi tetap 1.000.000
    const int simulations = 1000000;

    // Lakukan simulasi Monte Carlo
    double* simulatedPrices = nullptr;
    monteCarloSimulation(latestPrice, mean, standarDeviasi, months, simulations, simulatedPrices);

    // Hitung rata-rata hasil simulasi
    double predictedPrice = 0.0;
    for (int i = 0; i < simulations; ++i) {
        predictedPrice += simulatedPrices[i];
    }
    predictedPrice /= simulations;

    // Tampilkan hasil prediksi
    cout << "Prediksi harga emas setelah " << months << " bulan: " << predictedPrice << endl;

    delete[] prices;
    delete[] logReturns;
    delete[] simulatedPrices;

    return 0;
}
