#include <stdio.h>
#include <cuda.h>

typedef char DensityType;
typedef float4 VelocityType;

cudaArray* d_densityPingArray = 0;
cudaArray* d_densityPongArray = 0;
cudaArray* d_velocityPingArray = 0;
cudaArray* d_velocityPongArray = 0;

DensityType* d_densityPing;
DensityType* d_densityPong;
VelocityType* d_velocityPing;
VelocityType* d_velocityPong;

texture<DensityType, 3, cudaReadModeNormalizedFloat> densityTex; // 3D texture
texture<VelocityType, 3, cudaReadModeElementType> velocityTex; // 3D texture

cudaExtent volumeSize2;

extern "C" void checkCudaErr( const char* msg );
extern "C" void updateDensityTex(void* src);

__global__
void writeToPboKernel( char* pbo, DensityType* d_density, uint width, uint height, uint depth, int magic ) {
	int i = blockIdx.x % magic * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;
	int k = blockIdx.x / magic * blockDim.z + threadIdx.z;

	if (i < width && j < height && k < depth) {
		int idx = (k * (height * width) + j * width + i);
		pbo[idx] = d_density[idx];
	}
}
__global__
void diffuseDensityKernel( DensityType* data, DensityType* data0, float diff, float dt, int domX, int domY, int domZ, int magic ) {

	int i = blockIdx.x % magic * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;
	int k = blockIdx.x / magic * blockDim.z + threadIdx.z;

	if (i > 1 && i < domX - 1 && j > 1 && j < domY - 1 && k > 1 && k < domZ - 1) {
		for (int iter = 0; iter < 20; iter++) {
			char tmp = (data0[k * (domY * domX) + j * domX + i] + diff * (data0[k * (domY * domX) + j * domX + (i - 1)] + data0[k * (domY * domX) + j * domX + (i + 1)] + data0[k
					* (domY * domX) + (j - 1) * domX + i] + data0[k * (domY * domX) + (j + 1) * domX + i] + data0[(k - 1) * (domY * domX) + j * domX + i] + data0[(k + 1) * (domY
					* domX) + j * domX + (i + 1)])) / (1 + 6 * diff);

			data[k * (domY * domX) + j * domX + i] = min(250, tmp);
		}
		//data[k * (domY * domX) + j * domX + i] = 200;
	} else {
		data[k * (domY * domX) + j * domX + i] = 0;
	}
}


__global__
void advectDenistyKernel( DensityType* data, VelocityType* velocity, float dt, int domX, int domY, int domZ, int magic ) {

	int i = blockIdx.x % magic * blockDim.x + threadIdx.x;
	int j = blockIdx.y * blockDim.y + threadIdx.y;
	int k = blockIdx.x / magic * blockDim.z + threadIdx.z;

	if (i < domX && j < domY && k < domZ) {

/*
		float x = i - dt * velocity[k * (domY * domX) + j * domX + i].x;
		float y = j - dt * velocity[k * (domY * domX) + j * domX + i].y;
		float z = k - dt * velocity[k * (domY * domX) + j * domX + i].z;
		*/
		float x = i - dt * 0;
		float y = j - dt * 0;
		float z = k - dt * 0;

		float sample = tex3D(densityTex, x, y, z);
		data[k * (domY * domX) + j * domX + i] = (DensityType)(sample * 255);

	}
}

extern "C" void initCfd( char* h_volume, void* h_velocity, cudaExtent volumeSize ) {

	volumeSize2 = volumeSize;


	size_t size = volumeSize.height * volumeSize.width * volumeSize.depth;

	cudaMalloc((void**) &d_densityPing, size * sizeof(DensityType));
	cudaMalloc((void**) &d_densityPong, size * sizeof(DensityType));
	cudaMalloc((void**) &d_velocityPing, size * sizeof(VelocityType));
	cudaMalloc((void**) &d_velocityPong, size * sizeof(VelocityType));

	cudaMemcpy(d_densityPing, h_volume, size * sizeof(DensityType), cudaMemcpyHostToDevice);
	cudaMemcpy(d_densityPong, h_volume, size * sizeof(DensityType), cudaMemcpyHostToDevice);
	cudaMemcpy(d_velocityPing, h_velocity, size * sizeof(VelocityType), cudaMemcpyHostToDevice);
	cudaMemcpy(d_velocityPong, h_velocity, size * sizeof(VelocityType), cudaMemcpyHostToDevice);

	/** tex bussiness */
	cudaChannelFormatDesc densityChannelDesc = cudaCreateChannelDesc<DensityType> ();
	cudaMalloc3DArray(&d_densityPingArray, &densityChannelDesc, volumeSize);
	cudaMalloc3DArray(&d_densityPongArray, &densityChannelDesc, volumeSize);

	cudaChannelFormatDesc velocityChannelDesc = cudaCreateChannelDesc<VelocityType> ();
	cudaMalloc3DArray(&d_velocityPingArray, &velocityChannelDesc, volumeSize);
	cudaMalloc3DArray(&d_velocityPongArray, &velocityChannelDesc, volumeSize);

	cudaMemcpy3DParms copyParams = { 0 };
	copyParams.srcPtr = make_cudaPitchedPtr(h_volume, volumeSize.width * sizeof(DensityType), volumeSize.width, volumeSize.height);
	copyParams.dstArray = d_densityPingArray;
	copyParams.extent = volumeSize;
	copyParams.kind = cudaMemcpyHostToDevice;
	cudaMemcpy3D(&copyParams);

	copyParams.dstArray = d_densityPongArray;
	//cudaMemcpy3D(&copyParams);

	copyParams.srcPtr = make_cudaPitchedPtr(h_velocity, volumeSize.width * sizeof(VelocityType), volumeSize.width, volumeSize.height);
	copyParams.dstArray = d_velocityPingArray;
	cudaMemcpy3D(&copyParams);

	copyParams.dstArray = d_velocityPongArray;
	//cudaMemcpy3D(&copyParams);

	checkCudaErr("Kopiranje arrayev");

	densityTex.filterMode = cudaFilterModeLinear;
	velocityTex.filterMode = cudaFilterModeLinear;

	cudaBindTextureToArray(densityTex, d_densityPingArray, densityChannelDesc);
	cudaBindTextureToArray(velocityTex, d_velocityPingArray, velocityChannelDesc);

	checkCudaErr("Bindanje textur");

}

extern "C" void simulate( dim3 gridSize, dim3 blockSize, char* pbo, cudaExtent volumeSize, int magic ) {


	float dt = 0.05f;

	// delamo diffuse v pong rezultat
	diffuseDensityKernel<<<gridSize, blockSize>>>( d_densityPing, d_densityPong, 0.10f, dt, volumeSize.width, volumeSize.height, volumeSize.depth, magic);
	updateDensityTex(d_densityPing); // iz ping arraya v ping teksturo
/*
	// pingpong
	DensityType* tmp = d_densityPing;
	d_densityPing = d_densityPong;
	d_densityPong = tmp;
*/
	advectDenistyKernel<<<gridSize, blockSize>>>(d_densityPing, d_velocityPing, dt, volumeSize.width, volumeSize.height, volumeSize.depth, magic );
	//updateDensityTex(d_densityPing); // iz ping arraya v ping teksturo
	writeToPboKernel<<<gridSize, blockSize>>>(pbo, d_densityPing, volumeSize.width, volumeSize.height, volumeSize.depth, magic);


	// pingpong
	DensityType* tmp = d_densityPing;
	d_densityPing = d_densityPong;
	d_densityPong = tmp;

	cudaThreadSynchronize();
	checkCudaErr("writeToPboKernel");
}

extern "C" void updateDensityTex(void* src) {
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<DensityType> ();

	// copy data to 3D array
	cudaMemcpy3DParms copyParams = { 0 };
	copyParams.srcPtr = make_cudaPitchedPtr(src, volumeSize2.width * sizeof(DensityType), volumeSize2.width, volumeSize2.height);
	copyParams.dstArray = d_densityPingArray;
	copyParams.extent = volumeSize2;
	copyParams.kind = cudaMemcpyDeviceToDevice;
	cudaMemcpy3D(&copyParams);
	checkCudaErr("updateDensityTex failed");
}

extern "C" void checkCudaErr( const char* msg ) {
	cudaError_t err = cudaGetLastError();

	if (err != cudaSuccess) {
		fprintf(stderr, "\nCUDA error: %s (%s)\n", msg, cudaGetErrorString(err));
	}
}
