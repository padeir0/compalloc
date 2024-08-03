# compalloc

All allocators manage a piece of memory given to them,
they are not global and are not thread-safe,
unless it's the user desire to do so.

Important aspects:
 - Composable: it should be entirely possible to allocate a chunk from a free list allocator,
and set up another allocator in that region, be it a stack/pool/free-list/etc allocator;
 - Local: allocators should not have global context unless the user wants to do so;
 - Modular: each allocation algorithm should exist by themselves, completely separate;
 - Simple: no allocation algorithm shall possess enough complexity to bite us in the foot;
 - Tested: all allocators shall be throughly tested, nobody wants do debug memory bugs.


## Why?

Most allocators are global in nature, we can't segregate memory
in a way that it's possible for a section of a program to run out
of memory without impacting the whole program. This feature of locality
is specially useful for writing interpreters, the main program may
run the interpreter in a segmented part of memory, making it impossible
for the interpreter to consume memory of the entire application.

Most allocators are not composable, being global, they do not work well
together. Here, we should be able to have a pool of memory chunks, where
each memory chunk is managed by a stack allocator, if that is our desire
to do so.

Most allocators support a single algorithm, which often is very complex.
This library intends do shuffle the complexity to the end user, making
the user decide the algorithms, segments and how these are composed together.
It assumes there are no single best solution to all programs, as such,
a single allocator will not serve all programmer needs.
