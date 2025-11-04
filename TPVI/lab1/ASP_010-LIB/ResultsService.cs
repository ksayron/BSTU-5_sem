using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace BSTU.Results.Collection
{
    public class ResultsService : IResultsService
    {
        private readonly string _filePath;
        private object _lockObject = new Object();
        public ResultsService(string filePath)
        {
            _filePath = filePath;
            Directory.CreateDirectory(Path.GetDirectoryName(_filePath) ?? ".");

            if (!File.Exists(_filePath))
            {
                File.WriteAllText(_filePath, "[]", Encoding.UTF8);
            }
        }

        public void SaveJSON(List<Result> results)
        {
            var json = JsonSerializer.Serialize(results);
            File.WriteAllText(_filePath, json);
        }

        public List<Result> ReadJSON()
        {
            var file = File.ReadAllText(_filePath);
            var list = JsonSerializer.Deserialize<List<Result>>(file);
            return list;
        }

        public async Task<Result> AddResultAsync(string message)
        {
            lock(_lockObject)
            {
                var list = ReadJSON();
                var next = (list.Count == 0) ? 1 : (list.Max(x => x.Id) + 1);
                var newResult = new Result { Id = next, Message = message };
                list.Add(newResult);
                SaveJSON(list);
                return newResult;
            }
        }

        public async Task<bool> DeleteResultAsync(int id)
        {
            lock (_lockObject)
            {
                var list = ReadJSON();
                var item = list.FirstOrDefault(x => x.Id == id);
                if (item == null) return false;
                list.Remove(item);
                SaveJSON(list);
                return true;
            }
        }

        public async Task<IEnumerable<Result>> GetAllAsync()
        {
            lock (_lockObject)
            {
                return ReadJSON();
            }
            
        }

        public async    Task<Result> GetResultByIdAsync(int id)
        {
            var list = ReadJSON();
            return list.FirstOrDefault(x => x.Id == id);
        }

        public async Task<Result> UpdateResultAsync(int id, string message)
        {
            var list = ReadJSON();
            var item = list.FirstOrDefault(x => x.Id == id);
            if (item == null) return null;
            item.Message = message;
            SaveJSON(list);
            return item;
        }
    }
}
