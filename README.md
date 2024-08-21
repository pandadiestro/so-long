# So long
*and thanks for all the service, heh*

<div style="text-align: center;">
![](./doc/media/callao.png)
</div>

> *En el mundo en que yo vivo, siempre hay cuatro esquinas*

## What am i looking at?

The Callao port, 18th century, just like in this metaphorical **port** you may have some service resting in your network. If you want to land your own ship there you will have to let the first one go, hence so long!

Basically, if my cool kid analogy wasn't enough for you to get it, you know that moment when you want to run some service of yours at some specific port but said port has an already-running service in it so you run `npx kill-port <PORT>`, yeah that's it.

So-long is a TCP[6] and UDP[6] process communication interface (or process killer) which allows to send a POSIX [`SIGNAL`](https://dsa.cs.tsinghua.edu.cn/oj/static/unix_signal.html) to all running network processes connected to a certain port of your local network.

## How 2 use?

```
so-long
    -p [port | mandatory!]
    -s [signal | default:9]
    -P (print-mode | default:0)
    -v (verbose-mode | default:0)
```

Where `port` is the only mandatory argument and `signal` represents any valid signal (check your `/usr/include/signal.h`, `/usr/include/asm/signal.h` and `/usr/include/asm/siginfo.h` header files for valid signal int codes). Also, as per right now, the `verbose-mode` flag does nothing at all, it should print more information or something but I'll see what the best fit is for that flag.

The `-P` flag (print-mode) will prevent so-long from sending any signal at all, instead, it will print a, separated by newlines, list of the PIDs running in the specified port.

## Any future plans?

Yeah this was initially tought as a drop-in replacement for the kill-port npm package, so there's still a long way ahead before I can distribute this as such. By now, it will stay as a stand-alone POSIX-C-compliant application (sorry winheads ¯\\\_(ツ)\_/¯).

What I'm going to be actively looking in to is actually being able to compile this as a distributable WASM blob (as per the npm package problem).

