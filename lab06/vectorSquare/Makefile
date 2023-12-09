TARGET      = vectorSquare
SRC         = $(TARGET).cu

# 2023 Calvin CS Lab Nvidia GPU-architecture flags 
# Choose one CUDA_FLAGS definition based on the lab you are using
#   Gold Lab (RTX 3060, Ampere arch.): uncomment the next line 
CUDA_FLAGS  = -gencode arch=compute_86,code=sm_86 
#   Maroon Lab (GTX 1050Ti Pascal arch.): uncomment the next line 
#CUDA_FLAGS  = -gencode arch=compute_61,code=sm_61 
#   Systems Lab (GTX 960, Maxwell arch.): uncomment the next line 
#CUDA_FLAGS  = -gencode arch=compute_52,code=sm_52 

NVCC        = nvcc
# If you get an 'nvcc: not found' error when running 'make',
#  comment out the NVCC line above and uncomment the next 2 lines...
#CUDA_HOME   = /usr/local/cuda/bin/
#NVCC        = $(CUDA_HOME)nvcc

CFLAGS      = -Werror all-warnings
LFLAGS      = -o $(TARGET) -lgomp

$(TARGET): $(SRC)
	$(NVCC) $(SRC) $(CFLAGS) $(CUDA_FLAGS) $(LFLAGS)

clean: 
	rm -f $(TARGET) *.o *~ *#

