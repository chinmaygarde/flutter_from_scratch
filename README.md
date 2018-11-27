Notes:
------
* This is the companion repository for the `Flutter on Raspberry Pi (mostly) from Scratch` article.
* This repository is only meant to showcase what a minimal embedder might look like. It is not production ready (and never will be)!
* Though this repository is based off of the [Raspberry Pi Cross Compilation SDK and GN Sysroot](https://github.com/chinmaygarde/raspberrypi_cross_compilation_sdk) repository. You dont have to use this toolchain or GN.

Flutter on Raspberry Pi
=======================

Flutter Embedder for Pi using Broadcom APIs directly.

Prerequisites
-------------

* The `libflutter_engine.so`, `icudtl.dat` and `flutter_embedder.h` files for the Raspberry Pi placed in the `out/` directory.

Usage for Raspberry Pi
----------------------

* Download the prepared toolchain, sysroot and related tools to the `out` directory `./tools/setup_sdk.sh`.
  * This takes a while and downloads upto 1 GB of data from cloud storage.
* Prepare the build output directory `out` with paths to your toolchain using `./tools/setup_gn.sh`.
* Build using `ninja -C out` on your host.
  * Hack and repeat.
* Push your executable to the Raspberry Pi and run.
  * You should probably mount the `out` directory to the remote Raspberry Pi using SSHFS. That way, the build artifacts automatically end up getting pushed to the Pi.
