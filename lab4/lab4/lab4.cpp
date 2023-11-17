#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>
#include <thread>

#include <tchar.h>
#include <Windows.h>
#include <timeapi.h>

#include "BitmapPlusPlus.h"
#pragma comment(lib, "winmm.lib")

using namespace std;
using namespace bmp;

using Threads = unique_ptr<HANDLE[]>;
ofstream outputFile("output.txt", ios::out);
int start;

struct ProcessBitmapInfo
{
    Bitmap* image;
    unsigned lineNumber;
    size_t lineHeight;
};

Pixel Average(vector<optional<Pixel>> const& v)
{
    auto const count = v.size();
    int sumR = 0;
    int sumG = 0;
    int sumB = 0;
    int pixelsCount = 0;

    for (auto const& pixel : v)
    {
        if (!pixel)
        {
            continue;
        }
        pixelsCount++;
        sumR += pixel->r;
        sumG += pixel->g;
        sumB += pixel->b;
    }

    return Pixel(sumR / pixelsCount, sumG / pixelsCount, sumB / pixelsCount);
}

Pixel GetAverageColor(Bitmap const& img, int x, int y, CONST LPVOID lpParam)
{
    int myNumber = (int)(INT_PTR)lpParam;

    vector<optional<Pixel>> pixels = {
        img.get(x, y),
        img.get(x, y - 1),
        img.get(x, y + 1),
        img.get(x - 1, y),
        img.get(x - 1, y - 1),
        img.get(x - 1, y + 1),
        img.get(x + 1, y),
        img.get(x + 1, y - 1),
        img.get(x + 1, y + 1),
    };

    int end = timeGetTime();
    double millisec = (end - start) / ((double)CLOCKS_PER_SEC / 1000);
    outputFile << myNumber + 1 << " " << millisec << endl;

    return Average(pixels);
}

DWORD WINAPI BlurBitmap(CONST LPVOID lpParam)
{
    auto data = reinterpret_cast<ProcessBitmapInfo*>(lpParam);

    unsigned startY = data->lineNumber * data->lineHeight;

    auto image = data->image;
    unsigned imageWidth = image->width();

    for (unsigned i = 0; i < 30; i++)
    {
        for (unsigned y = startY; y < startY + data->lineHeight; ++y)
        {
            for (unsigned x = 0; x < imageWidth; ++x)
            {
                image->set(x, y, GetAverageColor(*image, x, y, lpParam));
            }
        }
    }

    delete data;
    ExitThread(0);
}


Threads CreateThreads(size_t count, function<ProcessBitmapInfo* (int)> dataCreatorFn)
{
    auto threads = make_unique<HANDLE[]>(count);

    for (unsigned i = 0; i < count; i++)
    {
        threads[i] = CreateThread(
            NULL, 0, &BlurBitmap, dataCreatorFn(i), CREATE_SUSPENDED, NULL
        );
    }

    return threads;
}

void SetCoresLimit(size_t limit)
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    size_t maxCoresCount = sysinfo.dwNumberOfProcessors;

    if (limit > maxCoresCount)
    {
        cout << "Max cores count is " << maxCoresCount << endl;
        limit = maxCoresCount;
    }

    auto procHandle = GetCurrentProcess();
    DWORD_PTR mask = static_cast<DWORD_PTR>((pow(2, maxCoresCount) - 1) / pow(2, maxCoresCount - limit));

    SetProcessAffinityMask(procHandle, mask);
}

int main(int argc, char* argv[])
{

    string inputFileName = "input1.bmp";
    string outputFileName = "output.bmp";
    int threadsCount = 3;
    int coresCount = 1;

    try {
        int temp;
        std::cin >> temp;
        start = timeGetTime();

        vector<int> priorities =
        {
            2,
            1,
            0
        };

        SetCoresLimit(coresCount);
        auto startTime = chrono::high_resolution_clock::now();
        Bitmap* image = new Bitmap(inputFileName);
        unsigned lineHeight = image->height() / threadsCount;

        auto threads = CreateThreads(threadsCount, [lineHeight, image](unsigned threadNumber) {
            return new ProcessBitmapInfo{
                image,
                threadNumber,
                lineHeight,
            };
            });

        SetThreadPriority(threads[0], 1);
        SetThreadPriority(threads[1], 0);
        SetThreadPriority(threads[2], -1);

        for (int i = 0; i < threadsCount; i++)
        {
            ResumeThread(threads[i]);
        }

        WaitForMultipleObjects(threadsCount, threads.get(), true, INFINITE);

        image->save(outputFileName);

        auto endTime = std::chrono::high_resolution_clock::now();
        chrono::duration<double> duration = endTime - startTime;

        cout << "Processing time: " << duration.count() * 1000 << " milliseconds" << std::endl;
    }
    catch (const bmp::Exception& e)
    {
        cout << "[BMP ERROR]: " << e.what() << endl;
        return -1;
    }

    return 0;
}

