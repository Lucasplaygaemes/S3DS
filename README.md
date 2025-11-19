# S3DS
S3DS stands for Ultra-Sonic 3D Scanner, The main objective of this code is a proof of concept to make a 3D scanner with Ultra-Sonic sensors and to study about C matrixes/arrays manipulation and C communication with an Arduino and 3D rendering with raylib.


# How It Works?
Or how it should work, the concept is that the sensor can see a "point", and tell its distance, with the mpu, if its has arised or descended, and then know how much with some math, how much is have moved, with that, start to make a 3d Model of the object, using a matrix, this is only for learning purposes.

# Install
Flash trough the Arduino IDE the file sender.c, or with any tool of your like.
Then compile and run the serial_sender.c and with new updates, the mesh.c to show with raylib a mesh 3D graph. 

Only one file need to be compile and for now it's the render.c, you can compile with:

```gcc render.c -o render -Iraylib/src -Lraylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11```

# Future
This is a proto-type, will be added a sensor accelerometer and 3-Axis Gyroscope Mpu-6050 Gy-521 for the arduino.
And more 2 will be added too, so it will scan 3 sides at the same time, and more if needed in the future.
Theres a lot to do yet, it's only a skecth of it.
