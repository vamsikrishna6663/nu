> cmus and the like just weren’t cutting it for me, so I wrote µ *[sic]*.
— minitech

ν is a small control-thing designed to manage [SoX play][SoX]. It was originally
written in Node, as I am not a competent C programmer, but it seemed wrong
that it should take up more memory than Vim doing basically nothing.

Building:

    $ vim config.h
    $ make CC=musl-gcc # Substitute gcc, clang, whatever

Running:

    $ mkfifo control
    $ ./ν
    $ echo -n "Tina Turner - Private Dancer/Tina Turner - 02 - What's Love Got To Do With It.ogg" >> control
    $ echo -n pause >> control
    $ echo -n play >> control

Commands:

- `pause`: Sends SIGSTOP to the music player.
- `play`: Sends SIGCONT to the music player.
- `toggle`: `pause` if playing, `play` if paused.
- `<some path>`: Sends SIGHUP to the music player and spawns a new music player, passing the combination of `MUSIC_DIR` with the path.

I like using it with [i3] and [dmenu]:

    bindsym $mod+p exec "(cd ~/music; find) | grep '\.ogg$' | sed 's/^..//' | dmenu -i | perl -pe 'chomp if eof' >> ~/.music-control"
    bindsym XF86AudioPlay exec "echo -n toggle >> ~/.music-control"

It’s not quite perfect yet — it uses EOF instead of a newline on a FIFO
to separate commands, for example — but I’m working on that 😊

Upcoming possible features include:

- An actual queue and things like that
- Support for queuing entire folders (wow!)
- Some kind of status interface

[SoX]: http://sox.sourceforge.net/
[i3]: http://i3wm.org/
[dmenu]: http://tools.suckless.org/dmenu/
