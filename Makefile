SHELL = /bin/bash

CUDAPREFIX = cuda
CPUPREFIX = cpu

OBJECTS = main.o adwt.o sysutil.o
CPU_OBJECTS = lpw.o combine.o denoise.o
CUDA_OBJECTS = lpw.o

OBJDIR = obj

CPUPROG = adwt_cpu
CUDAPROG = adwt_cuda

VPATH = src

CPPFLAGS = -O3 -Wall

cpu : $(addprefix ${OBJDIR}/, \
	$(addprefix ${CPUPREFIX}_, ${OBJECTS} ${CPU_OBJECTS}))
	g++ ${CPPFLAGS} -o ${CPUPROG} $^

cuda : $(addprefix ${OBJDIR}/, \
	$(addprefix ${CPUPREFIX}_, ${OBJECTS}) $(addprefix ${CUDAPREFIX}_, ${CUDA_OBJECTS}))
	nvcc ${CPPFLAGS} -o ${CUDAPROG} $^

${OBJDIR}/${CPUPREFIX}_%.o : %.cpp
	g++ ${CPPFLAGS} -Isrc -c -o $@ $<

${OBJDIR}/${CUDAPREFIX}_%.o : %.cu
	nvcc ${CPPFLAGS} -Isrc -c -o $@ $<

clean : 
	rm ${OBJDIR}/* ${CPUPROG}
