main: main.c
	gcc main.c -o main `pkg-config --cflags --libs gstreamer-1.0`

clean:
	rm main