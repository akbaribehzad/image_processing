#include "Image.h"
#include "PrintOnScreen.h"
#include <thread>
#include <iostream>
#include <chrono>

// Worker function for threading
void worker(Image* img, Color c, int x1, int y1, int x2, int y2)
{
	img->SetColorPartial(c, x1, y1, x2, y2);
}

// Determine coordinates of slices based on the desired number of threads
int AssignCoordiates(int x_co1[], int y_co1[], int x_co2[], int y_co2[], int width, int height, int numSlices)
{

	if (width % numSlices != 0)
	{
		std::cout << "Dimention must be a multiple of 2";
		return -1;
	}

	int wSliceSize = width / numSlices;
	std::cout << " wSliceSize = " << wSliceSize << "\n";

	int w = wSliceSize;

	x_co1[0] = 0;
	y_co1[0] = 0;

	x_co2[0] = wSliceSize;
	y_co2[0] = height;


	for (int i = 1; i < numSlices; i++)
	{
		x_co1[i] = w + 1;
		y_co1[i] = 0;

		x_co2[i] = w + wSliceSize;
		y_co2[i] = height;

		if(wSliceSize < width)
		{
			w += wSliceSize;
		}
		
	}
	return 0;
}

int main()
{
	// to store the threads
	// For testing purposes I have chosen one large file.
	// At first I the color of the entire file is changed and
	// the elapsed time is calculated and printed on the screen.

	// Then the image is divided into 4 slices and each slice is fed to
	// a single thread. The time elaped for executing all four threads
	// is computed and printed on the screen.

	// As it is noted, the execution time difference between the two
	// methods is tremendous.

	// Coordinates of the image
	int x_co1[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	int y_co1[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

	int x_co2[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };
	int y_co2[16] = { 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 };

	int code = AssignCoordiates(x_co1, y_co1, x_co2, y_co2, 3840, 2160, 16);
	if (code == -1)
	{
		return 0;
	}

	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

	// for TESTING purposes, create a 600X400 image
	//const int width = 600;
	//const int height = 400;
	//Image MyImage(width, height);

	//for (int y = 0; y < height; y++)
	//{
	//    for (int x = 0; x < width; x++)
	//    {
	//        MyImage.SetColor(Color((float)x / (float)width, 1.0f - ((float)x / (float)width), (float)y / (float)height), x, y);
	//    }
	//}

	//MyImage.Export("Large.bmp");

	Image ReadImage(0, 0);

	ReadImage.Read("3840-2160-sample.bmp");

	// Changing the color of the entire file
	auto t1 = high_resolution_clock::now();
	int x1 = 0, y1 = 0, x2 = 3840, y2 = 2160;
	ReadImage.SetColorPartial(Color(0, 102, 204), x1, y1, x2, y2);
	auto t2 = high_resolution_clock::now();
	auto ms_double2 = duration_cast<milliseconds>(t2 - t1);

	std::cout << "--------- Execution time for main thread ---\n";
	std::cout << ms_double2.count() << "ms\n";
	std::cout << "--------------------------------------------\n";

	ReadImage.Export("3840-2160-sample_full.bmp");

	int num_threads = 4;
	int arry_width = 3840;

	// Changing image color using Multithreading
	ReadImage.Read("3840-2160-sample.bmp");

	Color cl[16] = { 
		{0, 102, 204}, {0, 30, 210} , {50, 100, 250}, {110, 20, 229},
		{0, 102, 204}, {0, 30, 210} , {50, 100, 250}, {110, 20, 229},
		{0, 102, 204}, {0, 30, 210} , {50, 100, 250}, {110, 20, 229},
		{0, 102, 204}, {0, 30, 210} , {50, 100, 250}, {110, 20, 229},
	};

	std::cout << "First we will try with 4 threads\n";
	std::cout << "--------------------------------\n";

	t1 = high_resolution_clock::now();
	std::vector<std::thread> threads;
	for (int i = 0; i < num_threads; i++) {
		std::thread th = std::thread(worker, &ReadImage, cl[i], x_co1[i], y_co1[i], x_co2[i], y_co2[i]);
		threads.push_back(std::move(th)); 
		std::cout << "Thread " << i + 1 << " started " << std::endl;
	}

	for (auto& th : threads) { 
		th.join();
	}

	t2 = high_resolution_clock::now();
	ms_double2 = duration_cast<milliseconds>(t2 - t1);

	std::cout << "--------- Execution time for 4 threads ---\n";
	std::cout << ms_double2.count() << "ms\n";
	std::cout << "--------------------------------------------\n";

	ReadImage.Export("3840-2160-sample_copy.bmp");

	std::cout << "No we will try with 16 threads\n";
	std::cout << "--------------------------------\n";

	num_threads = 16;
	t1 = high_resolution_clock::now();

	std::vector<std::thread> new_threads;
	for (int i = 0; i < num_threads; i++) {
		std::thread th = std::thread(worker, &ReadImage, cl[i], x_co1[i], y_co1[i], x_co2[i], y_co2[i]);
		new_threads.push_back(std::move(th));
		std::cout << "Thread started" << std::endl;
	}

	for (auto& th : new_threads) {
		th.join();
	}

	t2 = high_resolution_clock::now();

	std::cout << "\n\n";
	std::cout << "--------- Execution time for 16 threads ---\n";
	std::cout << ms_double2.count() << "ms\n";
	std::cout << "--------------------------------------------\n";

	ReadImage.Export("3840-2160-sample_copy.bmp");

	// The display works fine for small images but it is not 
	// suitable for large files.

	//display("3840-2160-sample_copy.bmp");

	return 0;
}
