# semargl-ng
a set of CLI tools to perform spectral analysis of micromagnetics-specific data
___

## how to build:
assuming one has gcc, fftw, fftw-devel, fftw-libs-single installed:

`cd semargl-ng; make all`
___

## usage:

#### calculating mode profiles (point-wise FFT):

1. convert ovf data produced by either OOMMF or mumax3 to DUMP format, i.e. `mumax3-convert -dump *.ovf`
2. clean up nasty linux cache: `sync && echo 3 > /proc/sys/vm/drop_caches`
2. `t2wr2c *.dump` - transform data from t(ime) to w(frequency) domain assuming r(eal) input and output c(omplex) data. The data is saved into two set of files, namely *.dump.0 and *.dump.1 for amplitude and phase, respectively.
3. Append '.dump' to the filenames of all the output data, e.g. *.dump.0.dump. (this is temporary, till the order will be changed to *.0.dump/ *.1.dump).
4. Convert output *.dump files to VTK, e.g. `mumax3-convert -vtk=binary *.dump.0.dump`.
5. Visualize VTK files in your favorite software, e.g. paraview.

___
#### calculating dispersion:

comming soon...

## benefits:

- high performance
- can process data that does not fit RAM
- follows UNIX paradigm, i.e. each binary does only one thing and does it really well.
