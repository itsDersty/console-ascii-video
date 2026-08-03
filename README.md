# CAV video converter & player
`.cav` is a basic console video format. It doesn't support audio.
<img width="700" height="450" alt="cav player screenshot" src="https://github.com/user-attachments/assets/0b867c55-12cf-44d3-95c2-6f220e24607c" />

The converter is written in Python, and the player is written in C++.

### CAV file metadata struct (C/C++)
Comments contain a short description and an example value.
```cpp
struct CavMetadata {
    char magic[3];     // Constant magic word ("CAV")
    uint8_t version;   // Format version (1)
    uint8_t fps;       // Frames per second (60)
    uint16_t width;    // Video width in chars (80)
    uint16_t height;   // Video height in chars (71)
    uint32_t frames;   // Total frame count (1030)
    char title[64];    // Fixed 64-byte buffer for video name (any ASCII string shorter than 64 bytes)
};
```
Raw video data follows the metadata. It is RLE-compressed.

## Converter Usage:
To convert a video file to a CAV video, you need to run the `main.py` script with the provided parameters:
```python
main.py "original\video\path" "cav\file\folder" "optional video title"
```
This script will create a CAV file with a random UUID name.

## Player Usage:
The player currently supports Windows only.
To watch a CAV video file, first compile the `main.cpp` file using the C++20 standard or higher.
```bash
g++ main.cpp -o cavplayer.exe -std=c++26
```
Then you need to launch it with one argument: the CAV file path.
```bash
cavplayer.exe "path\to\cav\file.cav"
```

The repository already has demonstration videos in the `DemoVideos` folder.


This project was made just for fun. I don't have any plans to continue it in the future.
