# C11 threads compatibility wrapper for the macOS® operating system

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE.md)

This project provides a simple, light-weight replacement for absent C11 thread support (`threads.h`) in the macOS® operating system by wrapping calls to POSIX or other system calls instead.

**Note that not all C11 thread features are supported.**

While Clang, as distributed with the Xcode® development suite, does have C11 *language* support, the system's standard libraries need to support *non-language* C11 features as well, to be used. Support of C11 threads is one of those non-language features.

The code *may* also work for Linux, where it is generally should not be required on any half-way modern Linux distribution. Windows support is entirely out of scope and will not be added (I recommend using the wrapper that comes with [https://mesa3d.org/](Mesa) if you need that). If you seek support for more than just one platform, there are also libraries covering all operating systems in one go, so you may want to take a look at those instead. The goal of this small project is to just add compatibility for Mac® computers, e.g. if you have already figured everything out for other platforms.

... which was also the point of starting this project: I already had Linux (native support) and Windows (using Mesa's compatibility layer) implemented on my projects when I wanted to add Mac® compatibility and was surprised that C11 threads are not yet supported as of macOS® version 15 (2024). I didn't want to add another dependency, so I started writing my own wrapper. Well, uh, I guess now there's yet another one. :)

## How to use (recommendation)

You are free to just drop the source files into your project and adapt them as needed to simplify dependency management. Please mind the MIT license and copyright/license comment that is in those files. You are welcome to provide improvements back to this project as you see fit. This should also be preferred over copying modified versions across projects (it's better to share it in a central location like this repository).

If you make modifications to copies, it is highly recommended to mark any changes you make with start/end comments incl. author/year/reason (+ license) for traceability and to keep those sections separate from the general license and copyright.

Instead of cluttering all source files with preprocessor directives I would recommend to create and include a single file in your project, e.g. `threads_compat.h`, where you branch off into native `threads.h`, this `threads_macos_compat.h` (or even to `threads_macos_compat.c` from `threads_compat.c`) and a Windows compatibility layer, as needed. To figure out if you have native support you would want to check and add some define in your configuration; for example using CMake:

```cmake
INCLUDE(CheckIncludeFile)

CHECK_INCLUDE_FILE(threads.h HAVE_C11_THREADS)
IF(NOT HAVE_C11_THREADS)
    ADD_DEFINITIONS(-DNEED_C11_THREADS_WRAPPER)
ENDIF()
```

## License

All sources and original files of this project are provided under [MIT license](LICENSE.md), unless declared otherwise
(e.g. by source code comments). Please be aware that dependencies (e.g. libraries and/or external data used by this
project) are subject to their own respective licenses which can affect distribution, particularly in binary/packaged
form.

### Note on the use of/for AI

Usage for AI training is subject to individual source licenses, there is no exception. This generally means that proper
attribution must be given and disclaimers may need to be retained when reproducing relevant portions of training data.
When incorporating source code, AI models generally become derived projects. As such, they remain subject to the
requirements set out by individual licenses associated with the input used during training. When in doubt, all files
shall be regarded as proprietary until clarified.

Unless you can comply with the licenses of this project you obviously are not permitted to use it for your AI training
set. Although it may not be required by those licenses, you are additionally asked to make your AI model publicly
available under an open license and for free, to play fair and contribute back to the open community you take from.

AI tools are not permitted to be used for contributions to this project. The main reason is that, as of time of writing,
no tool/model offers traceability nor can today's AI models understand and reason about what they are actually doing.
Apart from potential copyright/license violations the quality of AI output is doubtful and generally requires more
effort to be reviewed and cleaned/fixed than actually contributing original work. Contributors will be asked to confirm
and permanently record compliance with these guidelines.

## Acknowledgements

Mac, macOS and Xcode are trademarks of Apple Inc., registered in the U.S. and other countries and regions.
