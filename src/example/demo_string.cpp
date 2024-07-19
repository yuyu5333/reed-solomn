#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include "reed_solomon.hpp"  // 确保你有正确的Reed-Solomon实现头文件

using namespace std;

#define RS_DATA_SHARDS_NUM		7
#define RS_PARITY_SHARDS_NUM	3
#define RS_SHARDS_NUM			10

int main(int argc, char *argv[])
{
    string inputString = "This is a test string for Reed-Solomon encoding and decoding.";
    vector<iovec*> origin;

    // 根据输入字符串长度计算每个数据分片的长度
    size_t perShardLen = (inputString.length() + RS_DATA_SHARDS_NUM - 1) / RS_DATA_SHARDS_NUM;

    cout << "Before the reconstruct:" << endl;
    for (int i = 0; i < RS_DATA_SHARDS_NUM; i++) {
        iovec *data = new iovec;
        size_t startIdx = i * perShardLen;
        size_t endIdx = min(startIdx + perShardLen, inputString.length());

        data->iov_base = new char[perShardLen + 1];
        data->iov_len = perShardLen;
        memset(data->iov_base, 0, perShardLen + 1);
        memcpy(data->iov_base, inputString.c_str() + startIdx, endIdx - startIdx);

        cout << (char*)data->iov_base << endl;
        origin.push_back(data);
    }
    cout << endl;

    ReedSolomon* rs = new ReedSolomon();
    rs->Initialize(RS_DATA_SHARDS_NUM, RS_PARITY_SHARDS_NUM);

    // 编码
    rs->Encode(origin);

    // 模拟丢失前三个数据分片
    cout << "Drop the first 3 data shards." << endl << endl;
    for (int i = 0; i < 3; i++) {
        iovec* v = origin[i];
        origin[i] = NULL;

        delete[] static_cast<char*>(v->iov_base);
        delete v;
    }

    // 重建
    rs->Reconstruct(origin);

    cout << "After the reconstruct:" << endl;
    string recoveredString;
    for (int i = 0; i < RS_DATA_SHARDS_NUM; i++) {
        if (origin[i] != NULL) {
            cout << (char*)origin[i]->iov_base << endl;
            recoveredString += string((char*)origin[i]->iov_base);
        }
    }

    cout << "Recovered string: " << recoveredString << endl;

    return 0;
}
