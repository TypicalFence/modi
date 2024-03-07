# Modi: Amiga Mod File Library & Tools

Modi is a C library for dealing with mod files, made by girls for girls.

It aims to be portable and supports protracker compatible 4 track mod files.

Additionally the project includes a set of tools that were written to test the
library.

The project is very work in progress.

## Library Requirements

- C99
  - stdint.h
- 16bit architecture
  - might port it to 8bit (probably not worth it tho)

## Building

The project can be build with the included Makefile, it has been built using gcc
and clang.

```bash
make
```

This will result in a static library called `modi.a`.

To build any of the tools you need to specify then when running make.

```
make modi-dump
```

## Tools

### modi-dump

A cli tool to extract samples from mod files.
