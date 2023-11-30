using System;
using System.IO;
using System.Net.Http;
using System.Text.Json;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        const string apiUrl = "https://dog.ceo/api/breeds/image/random";
        const int numberOfRequests = 10;

        Console.WriteLine("Асинхронная загрузка:");

        await DownloadImagesAsync(apiUrl, numberOfRequests);

        Console.WriteLine("\nСинхронная загрузка:");

        DownloadImagesSync(apiUrl, numberOfRequests);
    }

    static async Task DownloadImagesAsync(string apiUrl, int numberOfRequests)
    {
        var httpClient = new HttpClient();

        var downloadTasks = new Task[numberOfRequests];

        for (int i = 0; i < numberOfRequests; i++)
        {
            downloadTasks[i] = DownloadImageAsync(apiUrl, httpClient);
        }

        await Task.WhenAll(downloadTasks);
    }

    static void DownloadImagesSync(string apiUrl, int numberOfRequests)
    {
        var httpClient = new HttpClient();

        for (int i = 0; i < numberOfRequests; i++)
        {
            DownloadImageSync(apiUrl, httpClient).Wait();
        }
    }

    static async Task DownloadImageAsync(string apiUrl, HttpClient httpClient)
    {
        try
        {
            HttpResponseMessage response = await httpClient.GetAsync(apiUrl);

            response.EnsureSuccessStatusCode();

            string content = await response.Content.ReadAsStringAsync();
            string imageUrl = ParseJson(content);

            Console.WriteLine($"Starting download from URL: {imageUrl}");

            await DownloadAndSaveImageAsync(imageUrl);

            Console.WriteLine($"Image downloaded successfully: {imageUrl}");

        }
        catch (HttpRequestException ex)
        {
            Console.WriteLine($"Error downloading image from {apiUrl}: {ex.Message}");
        }
    }

    static async Task DownloadImageSync(string apiUrl, HttpClient httpClient)
    {
        try
        {
            HttpResponseMessage response = httpClient.GetAsync(apiUrl).Result;

            response.EnsureSuccessStatusCode();

            string content = await response.Content.ReadAsStringAsync();
            string imageUrl = ParseJson(content);

            Console.WriteLine($"Starting download from URL: {imageUrl}");

            DownloadAndSaveImageSync(imageUrl);

            Console.WriteLine($"Image downloaded successfully: {imageUrl}"); 
        }
        catch (HttpRequestException ex)
        {
            Console.WriteLine($"Error downloading image from {apiUrl}: {ex.Message}");
        }
    }

    static string ParseJson(string jsonString)
    {
        using (JsonDocument document = JsonDocument.Parse(jsonString))
        {
            return document.RootElement.GetProperty("message").GetString();
        }
    }

    static async Task DownloadAndSaveImageAsync(string imageUrl)
    {
        using (HttpClient client = new HttpClient())
        {
            byte[] imageBytes = await client.GetByteArrayAsync(imageUrl);
            string fileName = Path.GetFileName(imageUrl);
            await File.WriteAllBytesAsync(fileName, imageBytes);
        }
    }

    static void DownloadAndSaveImageSync(string imageUrl)
    {
        using (HttpClient client = new HttpClient())
        {
            byte[] imageBytes = client.GetByteArrayAsync(imageUrl).Result;
            string fileName = Path.GetFileName(imageUrl);
            File.WriteAllBytes(fileName, imageBytes);
        }
    }
}
