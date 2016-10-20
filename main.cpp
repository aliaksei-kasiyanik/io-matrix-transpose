#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

namespace {

    const string IN = "/Users/akasiyanik/FPMI/Tolstikov/io-matrix-transpose/in.bin";
    const string OUT = "/Users/akasiyanik/FPMI/Tolstikov/io-matrix-transpose/out.bin";
//    const string IN = "input.bin";
//    const string OUT = "output.bin";
    const int BLOCK = 2;

    void readBlockA(ifstream &in, int block_i, int block_j, int block_size, int block_n, int block_m, int n, int m,
            uint8_t *__restrict a) {
        int startA = 8;
        int shiftToNextRow = m - block_m;
        int block_start_position = startA + m * block_size * block_i + block_size * block_j;
        in.seekg(block_start_position, ios_base::beg);

        for (int i = 0; i < block_n; ++i) {
            if (i != 0) {
                in.seekg(shiftToNextRow, ios_base::cur);
            }
            char buffer[block_m];
            in.read(buffer, block_m);
            for (int j = 0; j < block_m; ++j) {
                a[i * block_m + j] = (uint8_t) buffer[j];
            }
        }

    }

    void writeBlock(ofstream &out, int block_i, int block_j, int block_size, int block_n, int block_m, int n, int m,
            uint8_t *__restrict a) {
        int startC = 8;

        int shiftToNextRow = m - block_m;

        int block_start_position = startC + m * block_size * block_i + block_size * block_j;
        out.seekp(block_start_position, ios_base::beg);

        for (int i = 0; i < block_n; ++i) {
            if (i != 0) {
                out.seekp(shiftToNextRow, ios_base::cur);
            }
            char buffer[block_m];
            for (int j = 0; j < block_m; ++j) {
                buffer[j] = a[i * block_m + j];
            }
            out.write(buffer, block_m);
        }
    }

    void calculateSizeOfBlock(int n, int m, int BLOCK, int i_block, int j_block, int &n_block, int &m_block) {
        if ((i_block + 1) * BLOCK > n) {
            n_block = n % BLOCK;
        } else {
            n_block = BLOCK;
        }
        if ((j_block + 1) * BLOCK > m) {
            m_block = m % BLOCK;
        } else {
            m_block = BLOCK;
        }
    }

    void BlockMult(int BLOCK) {
        ifstream in(IN, ios::in | ios::binary);
        ofstream out(OUT, ios::out | ios::binary);
        if (in.is_open() && out.is_open()) {
            int n, m;

            in.read((char *) &n, sizeof(n));
            in.read((char *) &m, sizeof(m));
            out.write((char *) &m, sizeof(m));
            out.write((char *) &n, sizeof(n));

            int blockCountInRow = (int) ceil((float) n / BLOCK);
            int blockCountInColumn = (int) ceil((float) m / BLOCK);
            for (int i = 0; i < blockCountInRow; ++i) {
                for (int j = 0; j < blockCountInColumn; ++j) {
                    int a_n, a_m;
                    calculateSizeOfBlock(n, m, BLOCK, i, j, a_n, a_m);
                    uint8_t *a = new uint8_t[a_n * a_m];
                    readBlockA(in, i, j, BLOCK, a_n, a_m, n, m, a);
                    uint8_t *b = new uint8_t[a_m * a_n];
                    //swap
                    for (int i_b = 0; i_b < a_n; ++i_b) {
                        for (int j_b = 0; j_b < a_m; ++j_b) {
                            b[j_b * a_n + i_b] = a[i_b * a_m + j_b];
                        }
                    }
                    delete[] a;
                    writeBlock(out, j, i, BLOCK, a_m, a_n, m, n, b);
                    delete[] b;
                }
            }
        }
    }


    void generateInFile(int n, int m) {
        ofstream file(IN, ios::out | ios::binary);
        if (file.is_open()) {
            file.write((char *) &n, sizeof(n));
            file.write((char *) &m, sizeof(m));
            for (int i = 0; i < n * m; i++) {
                uint8_t num = (uint8_t) (rand() % 256);
                file.write((char *) &num, sizeof(num));
            }
            file.close();
        }
    }

    void printInFile() {
        ifstream file(IN, ios::in | ios::binary);
        if (file.is_open()) {
            int n, m;
            cout << "MATRIX A" << endl;
            file.read((char *) &n, sizeof(n));
            file.read((char *) &m, sizeof(m));
            uint8_t *a = new uint8_t[n * m];
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    uint8_t num;
                    file.read((char *) &num, sizeof(num));
                    a[i * n + j] = num;
                    cout << unsigned(a[i * n + j]) << " ";
                }
                cout << endl;
            }
            file.close();
        }
    }

    void printOutFile() {
        ifstream file(OUT, ios::in | ios::binary);
        if (file.is_open()) {
            int n, m;

            cout << "MATRIX A TRANSPOSED" << endl;
            file.read((char *) &n, sizeof(n));
            file.read((char *) &m, sizeof(m));
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < m; ++j) {
                    uint8_t num;
                    file.read((char *) &num, sizeof(num));
                    cout << unsigned(num) << " ";
                }
                cout << endl;
            }
            file.close();
        }
    }
}


int main(int argc, char *argv[]) {
    generateInFile(2, 8);
    printInFile();
    BlockMult(3);
    printOutFile();
//    test();
    return 0;
}

