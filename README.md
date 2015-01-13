# semargl-ng
a set of CLI tools to perform spectral analysis of micromagnetics-specific data
___

## usage:
#### calculating mode profiles (point-wise FFT):

1. convert ovf data produced by either OOMMF of mumax3 to DUMP format, i.e. mumax3-convert -dump *.ovf
2. t2wr2c *.dump - transform data from t(ime) to w(frequency) domain assuming r(eal) input and outputting c(omplex) data. The data is saved into two set of files, namely *.dump.X and *.dump.Y for amplitude and phase, respectively.
3. Append '.dump' to the filenames of all the output data, e.g. *.dump.X.dump. (this is temporary, till the order will be changed to *.X.dump).
4. Convert output *.dump files to VTK, e.g. mumax3-convert -vtk=binary *.dump.X.dump.
5. Visualize VTK files in your favorite software, e.g. paraview.

___
#### calculating dispersion:

comming soon...
