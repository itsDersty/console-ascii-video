#define CAV_VERSION 0

#include <iostream>
#include <cstdint>
#include <fstream>
#include <windows.h>
#include <vector>
#include <span>

#pragma pack(push, 1)

using namespace std;

const char* CHARS = " .:-=+*#%@";


struct CavMetadata {
    char magic[3];     // "CAV"
    uint8_t version;   // 0
    uint8_t fps;       // 60
    uint16_t width;    // 80
    uint16_t height;   // 71
    uint32_t frames;   // 1030
};
#pragma pack(pop)

void goToXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Please provide a file.";
        return 1;
    }

    string file_path = argv[1];

    ifstream file(file_path, ios::binary);

    if (!file) {
        cout << "Invalid file.";
        return 1;
    }

    CavMetadata metadata;
    file.read(reinterpret_cast<char*>(&metadata), sizeof(metadata));

    if (metadata.version != CAV_VERSION) {
        cout << "CAV v"<<metadata.version<<" is not supported. This player supports CAV v"<<CAV_VERSION<<".";
        return 1;
    }

    if (metadata.magic[0]!='C' or metadata.magic[1]!='A' or metadata.magic[2]!='V') {
        cout << "Please provide valid CAV file.";
        return 1;
    }

    const size_t video_length = metadata.frames/metadata.fps;
    size_t current_frame = 0;

    const size_t chunkSize = metadata.width*metadata.height; 
    vector<uint8_t> buffer(chunkSize);

    while (file.read(reinterpret_cast<char*>(buffer.data()), chunkSize) || file.gcount() > 0) {
        streamsize bytesRead = file.gcount();

        goToXY(0,0);
        for (int i=0; i<metadata.height; ++i) {
            size_t row_start = i*metadata.width;
            
            span<uint8_t> row(buffer.data()+row_start,metadata.width);

            string row_string = "";
            row_string.reserve(metadata.width);

            for (uint8_t &c : row) {
                row_string += CHARS[c];
            }
            cout << row_string << '\n';
        }

        const size_t current_second = current_frame/metadata.fps;
        cout << current_second << " : " << video_length << " - Unknown video";
        
        Sleep(1000/metadata.fps);
        current_frame++;
    }

    return 0;
}