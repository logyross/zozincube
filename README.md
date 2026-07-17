# zozincube
A software rendered spinning wire cube in C++ inspired by tsoding's video

# usage
simply run the script at the root of the repository (`run.sh`). This will build and run the program which will create the individual animation frames.
After that, it will try to use `ffmpeg` to collect the frames into an mp4 video and play it with `mpv`.
If you only want to build the program you can simply run `make` or `make release` for release build or `make debug` for a debug build. (the script uses release build)
# Dependencies
## A C++17 compiler

## ffmpeg
The program uses `ffmpeg` to stitch together individual frames into a video. If you don't have it
installed, you can manually check the frames using a image editor. The frames are saved in `output/` as .ppm files.

## mpv
After the video is saved, it's played with mpv.
if you don't have mpv, you can manually play the video using a different video player (It's saved as output.mp4)

# instructions
- simply run "run.sh"

## handle point, handle line

- determine how a 3D point will reflect on 2D surface
- apply movement (rotation, translation)

# what should be

## project point, project line

## apply_rotation

## apply translation


