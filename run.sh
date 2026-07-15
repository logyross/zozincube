#!/bin/sh

ffmpeg -i output/frame-%d.ppm -r 60 output.mp4
rm output/*.ppm
mpv output.mp4