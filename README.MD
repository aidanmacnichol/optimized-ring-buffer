# Exploring a Ring Buffer with C++

## What is a Ring Buffer?

A ring buffer (also known as a circular buffer) is a single producer single consumer (SPSC), fixed sized FIFO data structure (mouth full I know).

In essence it works by keeping track of the head and tail of the data inside it and when the buffer gets full, it overrides the oldest data, like its connected end to end in a circle. Here is a helpful animation from wikipedia.

![circular buffer animation](/docs/Circular_Buffer_Animation.gif)

There are many use cases for ring buffer:

- Embedded Systems - I/O Communication
- Streaming Data
- Real-time sensor logging
- DSP delay lines

My favorite use case although, is in the [Herzberg Extensible Adaptive Real-time Toolkit](https://ao4elt6.copl.ulaval.ca/proceedings/401-C2S7-251.pdf) (HEART). Its a toolkit developed by the very smart people at the Herzberg Asronomy and Astrophysics Research Centre in Victoria, BC, Canada.

Basically this system removes atmosphyric distortion from ground based telescopes by literally measuring the distortion and moving thousands of small actuators attached the telescopes mirror to produce the "opposite" distortion all in real time. Similar to how noise cancelling headphones play the "opposite" sound to cancel out any incoming soundwaves, just 1000x cooler and 10,000 times harder.

Fast, efficient ring buffers are integral to this system for logging as well as interacting with hardware so I wanted to take some time to explore this datastructure and give a shot at optimizing it.
