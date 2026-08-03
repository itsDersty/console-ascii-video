#define CAV_VERSION 1

#include <iostream>
#include <cstdint>
#include <fstream>
#include <windows.h>
#include <vector>
#include <span>
#include <format>
#include <cstdlib>

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
    char title[64];    // Fixed 64-byte buffer for video name
};
#pragma pack(pop)

void setConsoleSize(int width, int height) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    SMALL_RECT minimalWindow = { 0, 0, 0, 0 };
    SetConsoleWindowInfo(hConsole, TRUE, &minimalWindow);

    COORD bufferSize = { (short)width, (short)height };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    SMALL_RECT windowSize = { 0, 0, (short)(width - 1), (short)(height - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void goToXY(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

string getTimeString(size_t rawSeconds) {
    size_t minutes = rawSeconds/60;
    size_t seconds = rawSeconds-(minutes*60);

    return format("{:02}:{:02}",minutes,seconds);
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

    setConsoleSize(metadata.width,metadata.height+5);
    std::system("cls");

    const size_t video_length = metadata.frames/metadata.fps;
    size_t current_frame = 0;

    const size_t frameSize = metadata.width*metadata.height; 
    std::uint8_t pair[2];

    string frame_buffer = "";
    frame_buffer.reserve(frameSize);

    size_t cur_row_pixel = 0;
    size_t cur_frame_pixel = 0;
    size_t cur_frame = 0;

    while (file.read(reinterpret_cast<char*>(pair), 2)) {
        uint8_t pixel_count = pair[0];
        uint8_t pixel_value = pair[1];

        for (size_t i=0; i<pixel_count; ++i) {
            cur_row_pixel++;
            cur_frame_pixel++;
            frame_buffer+=CHARS[pixel_value];

            if (cur_row_pixel >= metadata.width) {
                cur_row_pixel=0;
                frame_buffer+='\n';
            }
        }

        if (cur_frame_pixel >= frameSize) {
            cout << frame_buffer<<'\n';
            frame_buffer.clear();
            cur_frame_pixel=0;
            cur_row_pixel=0;
            cur_frame++;
            
            const size_t current_second = cur_frame/metadata.fps;
            cout << getTimeString(current_second) << " of " << getTimeString(video_length) << " | "<<metadata.title;

            goToXY(0,0);
            Sleep(1000/metadata.fps);
        }
    }
    
    return 0;
}