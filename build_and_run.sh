#!/bin/sh
if type "make" > /dev/null 2>&1; then
	make clean
	make
	if ./zozincube; then
		if type "ffmpeg" > /dev/null 2>&1; then
			ffmpeg -hide_banner -loglevel error -i frames/frame-%d.ppm -r 60 cube.mp4 
			if type "mpv" > /dev/null 2>&1; then
				mpv cube.mp4
			else
				echo "mpv not found, use your favourite video player to check the result (cube.mp4)"
			fi
		else
			echo "ffmpeg not found. Can't stitch the frames into a video."
		fi
	else
		echo "error occured during program execution."
	fi
else
	echo "make not found."
fi
