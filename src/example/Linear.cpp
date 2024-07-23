#include <iostream>
#include <vector>
#include <bitset>

const int k = 4;  // Number of data bits
const int n = 7;  // Number of coded bits

// Hamming (7,4) code generator matrix
int G[k][n] = {
    {1, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 0, 1, 0, 1},
    {0, 0, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 1, 1}
};

// Hamming (7,4) code parity-check matrix
int H[3][n] = {
    {1, 0, 1, 0, 1, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {0, 0, 0, 1, 1, 1, 1}
};

// Encode function
std::vector<std::bitset<8>> encode(const std::vector<std::bitset<8>>& data) {
    std::vector<std::bitset<8>> encoded;
    for (auto& byte : data) {
        for (int part = 0; part < 2; ++part) {
            std::bitset<k> segment((byte >> (part * k)).to_ulong() & 0xF);
            std::bitset<8> encoded_bits;  // Using 8 bits, highest bit will remain 0
            for (int i = 0; i < k; ++i) {
                if (segment[i]) {
                    for (int j = 0; j < n; ++j) {
                        // encoded_bits[j] ^= G[i][j];
                        encoded_bits[j] = encoded_bits[j] ^ G[i][j];
                    }
                }
            }
            encoded.push_back(encoded_bits);
        }
    }
    return encoded;
}

// Decode function
std::vector<std::bitset<8>> decode(const std::vector<std::bitset<8>>& encoded) {
    std::vector<std::bitset<8>> decoded;
    for (size_t i = 0; i < encoded.size(); i += 2) {
        std::bitset<8> byte;
        for (int part = 0; part < 2; ++part) {
            auto& bits = encoded[i + part];
            std::bitset<3> syndrome;
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < n; ++k) {
                    if (H[j][k] && bits[k]) {
                        syndrome[j] = !syndrome[j];
                    }
                }
            }

            // Error correction logic omitted for simplicity

            // Extract original data bits
            for (int j = 0; j < k; ++j) {
                byte[j + part * k] = bits[j];
            }
        }
        decoded.push_back(byte);
    }
    return decoded;
}

int main() {
    std::vector<std::bitset<8>> input = {std::bitset<8>("01011010"), std::bitset<8>("10110101")};
    auto encoded = encode(input);
    auto decoded = decode(encoded);

    std::cout << "Row: \t\t";
    for (auto& bits : input) {
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    std::cout << "Encoded: \t";
    for (auto& bits : encoded) {
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    std::cout << "Decoded: \t";
    for (auto& bits : decoded) {
        std::cout << bits << " ";
    }
    std::cout << std::endl;

    return 0;
}
