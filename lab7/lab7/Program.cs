using System;
using System.IO;
using System.Text;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("Введите путь к текстовому файлу:");
        string filePath = Console.ReadLine();

        try
        {
            string text = await ReadTextFromFileAsync(filePath);

            Console.WriteLine("Введите список символов для удаления (без пробелов):");
            string charactersToRemove = Console.ReadLine();

            text = RemoveCharacters(text, charactersToRemove);

            await SaveTextToFileAsync(filePath, text);

            Console.WriteLine("Операция завершена успешно.");
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Произошла ошибка: {ex.Message}");
        }
    }

    static async Task<string> ReadTextFromFileAsync(string filePath)
    {
        using (StreamReader reader = new StreamReader(filePath))
        {
            return await reader.ReadToEndAsync();
        }
    }

    static string RemoveCharacters(string text, string charactersToRemove)
    {
        foreach (char c in charactersToRemove)
        {
            text = text.Replace(c.ToString(), string.Empty);
        }

        return text;
    }

    static async Task SaveTextToFileAsync(string filePath, string text)
    {
        using (StreamWriter writer = new StreamWriter(filePath, false, Encoding.UTF8))
        {
            await writer.WriteAsync(text);
        }
    }
}
