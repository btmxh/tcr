# tcr

A script-to-image CPU renderer for Twitch chat.

Useful for people who wants to generate a Twitch chat without having to record from a real stream.

**This was used in the last colorist project of Ayumuian culture**

Note:
Many files in this repository is copyrighted, so only the code is in the public domain.

## How to build & run

The only dependency of this program is `freetype`, which can be easily installed on Unix
platforms. For Windows users, you gotta figure that out yourselves (conan, vcpkg, specify
the path, etc.).

To build, simply run `cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release && cmake --build build -j`.

WARNING: This might not work on your compilers, so you have to fix the code (it should be
trivial if you are somewhat proficient with C/C++, and if you aren't, good luck.)

If you want to change the font, replace the files in `res/fonts` (yes you need to replace
the files itself, use symlinks or something).

To add more emotes, add the emote image to `res/emotes` (WEBP not supported, so convert
the image to PNG/GIF first). Then, run `emotes-grabber.py` to register the emote to the
emotes.lib file (which is a text file with the worst extension choice ever).

## Why these shortcomings?

This project was written in the last days of the Kano-era, after the inevitable end of the
glorious Ayumu-era. Back then, I was a cheerful and happy person, full with confidence and
hope for the future. I wrote this program for a video (which was left unfinished), and the
repository was initially privated.

In July 2023, when new meta like nayuta and VTubers emerged, I realized the true beauty of
Ayumuian culture, digging up stuff from the past for archiving purposes. This project was
opened public as a tribute to the good old days.

tldr; here's the two reasons:
- This was meant to be a personal project.
- I was shit with programming back then.
