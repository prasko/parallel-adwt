SHELL = /bin/bash

OBJECTS = main.o adwt.o sysutil.o
CPU_OBJECTS = lpw.o combine.o denoise.o
CUDA_OBJECTS = lpw.o

BINDIR = bin

CPUPROG = adwt_cpu
CUDAPROG = adwt_cuda

VPATH = src

CPPFLAGS = -O3 -Wall

cpu : $(addprefix bin/, $(addprefix cpu_, ${OBJECTS} ${CPU_OBJECTS}))
	g++ ${CPPFLAGS} -o ${CPUPROG} $^

cuda : $(addprefix bin/, \
	$(addprefix cpu_, ${OBJECTS}) \ $(addprefix cuda_, ${CUDA_OBJECTS}))
	nvcc ${CPPFLAGS} -o ${CUDAPROG} $^

test : $(addprefix bin/, ici_denoiser_test)
	bin/ici_denoiser_test < data/noise1024.in > tmp
	diff tmp data/noise1024.std_rici.out	
	rm tmp

bin/ici_denoiser_test: bin/cpu_denoise.o src/test/ici_denoiser.cpp
	g++ ${CPPFLAGS} -Isrc -o $@ $^

bin/cpu_%.o : %.cpp
	g++ ${CPPFLAGS} -Isrc -c -o $@ $<

bin/cuda_%.o : %.cu
	nvcc ${CPPFLAGS} -Isrc -c -o $@ $<

clean : 
	rm ${BINDIR}/* ${CPUPROG}
