# semargl-ng
a set of CLI tools to perform spectral analysis of micromagnetics-specific data
___

## how to build:
Dependencies:
* gcc
* gsl
* openmp
* fftw
* fftw-devel
* fftw-libs-single

Install:
`cd semargl-ng; make all; sudo make install`
___

## usage:

#### calculating mode profiles (point-wise FFT):

1. convert ovf data produced by either OOMMF or mumax3 to DUMP format, i.e. `mumax3-convert -dump *.ovf`. Alternatively, one can instruct mumax3 to save dump files directly by setting `OutputFormat = DUMP`.
2. The data could be pre-processed to extract dynamic part of the magnetization vector and transforming it to *uvw* coordinates as given by the local direction of the magnetization vector in the ground state
: `mxyz2muvw *.dump`. This will output files suffixed with *.UVW.dump.
3. `t2wr2c *.(UVW.)dump` - transform (pre-processed) data from t(ime) to w(frequency) domain assuming r(eal) input and output c(omplex) data. The data is saved into two set of files, namely *.X.dump and *.Y.dump for amplitude and phase, respectively.
4. Convert output *.dump files to VTK, e.g. `mumax3-convert -vtk=binary *.X.dump`.
5. Visualize VTK files in your favorite software, e.g. paraview.

___
#### calculating dispersion:

comming soon...

## benefits:

- high performance
- can process data that does not fit RAM
- follows UNIX paradigm, i.e. each binary does only one thing and does it really well.
