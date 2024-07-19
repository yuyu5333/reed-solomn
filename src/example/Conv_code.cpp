#include <iostream>
#include <vector>
#include <bitset>
#include <cmath>
#include <algorithm>

using namespace std;

class ConvolutionalEncoder {
public:
    ConvolutionalEncoder() {
        shiftRegister = 0;
    }

    vector<bitset<2>> encode(const vector<bitset<8>>& input) {
        vector<bitset<2>> encodedBits;
        for (const auto& byte : input) {
            for (size_t i = 0; i < byte.size(); ++i) {
                bool inputBit = byte[i];
                shiftRegister = (shiftRegister << 1) | inputBit;

                bitset<2> outputBits;
                outputBits[0] = (shiftRegister & 0b101) != 0; // 示例生成第一个输出位
                outputBits[1] = (shiftRegister & 0b111) != 0; // 示例生成第二个输出位

                encodedBits.push_back(outputBits);
            }
        }
        return encodedBits;
    }

private:
    unsigned int shiftRegister;
};

class ViterbiDecoder {
public:
    ViterbiDecoder() {
        pathMetric = {0, INFINITY, INFINITY, INFINITY};
        state = 0;
    }

    vector<bitset<8>> decode(const vector<bitset<2>>& encodedInput) {
        vector<bitset<8>> decodedBytes;
        vector<int> path(encodedInput.size() + 1, -1);

        for (size_t i = 0; i < encodedInput.size(); ++i) {
            vector<double> newMetric(4, INFINITY);
            for (int s = 0; s < 4; ++s) {
                for (int input = 0; input < 2; ++input) {
                    int nextState = ((s << 1) | input) & 0b11;
                    bitset<2> expectedOutput;
                    expectedOutput[0] = (s & 0b1) ^ ((s >> 1) & 0b1);
                    expectedOutput[1] = (s & 0b1) ^ input;

                    double metric = pathMetric[s] + hammingDistance(encodedInput[i], expectedOutput);
                    if (metric < newMetric[nextState]) {
                        newMetric[nextState] = metric;
                        path[i + 1] = s;
                    }
                }
            }
            pathMetric = newMetric;
        }

        int bestState = min_element(pathMetric.begin(), pathMetric.end()) - pathMetric.begin();
        bitset<8> decodedByte;
        for (size_t i = encodedInput.size(); i > 0; --i) {
            int inputBit = path[i] >> 1;
            decodedByte[i % 8] = inputBit;
            if (i % 8 == 0) {
                decodedBytes.push_back(decodedByte);
                decodedByte.reset();
            }
        }
        reverse(decodedBytes.begin(), decodedBytes.end());
        return decodedBytes;
    }

private:
    double hammingDistance(const bitset<2>& a, const bitset<2>& b) {
        return (a[0] != b[0]) + (a[1] != b[1]);
    }

    vector<double> pathMetric;
    int state;
};

int main() {
    ConvolutionalEncoder encoder;
    vector<bitset<8>> input = {0b11011010, 0b10101100};
    vector<bitset<2>> encoded = encoder.encode(input);

    cout << "Encoded bits:" << endl;
    for (const auto& bits : encoded) {
        cout << bits << endl;
    }

    ViterbiDecoder decoder;
    vector<bitset<8>> decoded = decoder.decode(encoded);

    cout << "Decoded bytes:" << endl;
    for (const auto& byte : decoded) {
        cout << byte << endl;
    }

    return 0;
}
