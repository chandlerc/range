A Range STL Extension
=====================

This provides an extension to the standard library implementing a proposed
range-based collection of interfaces. The goal of this library is to provide
example code for a proposal to the standard library. A secondary goal is to
provide the basis of an implementation of these extensions for [libc++][1].
Eventually, this code will be proposed for inclusion into libcxx proper, and as
such the license requirements and some of the style elements follow its lead.
However, the style is optimized for readability in this early version, the
symbol hardening will take place later.

[1]: http://libcxx.llvm.org/

Large portions of this work were originally authored by Jeffrey Yasskin as part
of his [cxx1y-range][2] project. This is a direct continuation of that effort,
and specifically an attempt to more fully explore the set of changes to the
standard library needed to have a working range abstraction as a core part of
the standard library. Note that all of the code relating to the previous project
has been relicensed to be suitable for inclusion in libc++ and also to present
fewer barriers to other vendors as the MIT license is among the most liberal
available.

[2]: http://code.google.com/p/cxx1y-range
