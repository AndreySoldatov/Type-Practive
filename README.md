![Logo](https://i.ibb.co/Fsy15r0/icon.png)

# Type-Practive
Intuitive, Easy-to-use tool for laerning speed typing.

## Use this program to train...
![Screenshot 1](https://i.ibb.co/tpYQQDq/screen1.png)

## ...and track...
![Screenshot 2](https://i.ibb.co/fkdwpM3/screen2.png)

## your speed typing skill.

## IT DOES NOT WORK ON LINUX AND MAC-OS.

## Using:
* run the program to start typing.
* open the log files in "res/log/" folder with the executable to check your statistics
* to change the text simply change the contents of "res/text/"
* to change font replace the font file in "res/font/", but it's name needs to be "font.ttf"
and it would be better if the font you use would be monospaced.
* to change text in the program press TAB

## Building:
this code was build in the windows 10 x64 machine with minGW g++ compiler
and SFML library.

so if you have those installed you can build this program by typing:
**"g++ -W -std=c++17 -mwindows main.cpp -o TypePractive -lsfml-graphics -lsfml-window -lsfml-system my.res"**
