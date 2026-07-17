# zozincube
A software rendered spinning wire cube in C++ inspired by tsoding's video.


<img width="480" height="480" alt="cube" src="https://github.com/user-attachments/assets/b3ddd9b8-c2a5-4ae1-8519-b9d6c0710b67" />


# Usage
Simply run the script at the root of the repository (`build_and_run.sh`). This will build and run the program which will create the individual animation frames.
After that, it will try to use `ffmpeg` to collect the frames into an mp4 video and play it with `mpv`.
If you only want to build the program you can simply run `make` or `make release` for release build or `make debug` for a debug build. (the script uses release build)
# Dependencies
## A C++ compiler
Needs to be compatible with C++17

## ffmpeg
The program uses `ffmpeg` to stitch together individual frames into a video. If you don't have it
installed, you can manually check the frames using an image editor. The frames are numbered and saved in `frames/` as .ppm files.

## mpv
After the video is saved, it's played with mpv.
if you don't have mpv, you can manually play the video using a different video player (It's saved as `cube.mp4`).


# references
- original video by tsoding: https://www.youtube.com/watch?v=qjWkNZ0SXfo
- idea to use ppm files and ffmpeg for animation: https://www.youtube.com/watch?v=xNX9H_ZkfNE
- idea for finding all points that connect two coords: https://stackoverflow.com/questions/13491676/get-all-pixel-coordinates-between-2-points
