using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BSTU.Results.Collection
{
    public interface IResultsService
    {
        Task<IEnumerable<Result>> GetAllAsync();
        Task<Result> GetResultByIdAsync(int id);
        Task<Result> AddResultAsync(string message);
        Task<Result> UpdateResultAsync(int id,string message);
        Task<bool> DeleteResultAsync(int id);
    }


}
