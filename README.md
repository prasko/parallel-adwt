Parallel ADWT (adaptive discrete wavelet transformation)
========================================================

The project is the thesis for my B. Sc. diploma in University of
Zagreb, Faculty of Electrical Engineering and Computing (FER), and is
not currently open for contribution.


About the project
-----------------

Adaptive discrete wavelet transform is a modern method used in signal
processing for denoising, compression, etc. JPEG2000 is an example.

ADWT is implemented using filter banks which are the static part and
an adaptive part which tries to guess some parameters to improve the
algorithm. The goal is to contribute to ADWT by speeding up the
algorithm using CUDA architecture and to implement some new ideas
concerning the adaptive part.

It was previously implemented in Matlab, and I'm coding it in C++.

----------

Matija Osrecki
