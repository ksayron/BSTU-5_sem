using Microsoft.AspNetCore.Http;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace BSTU.Results.Authenticate
{
    public interface IAuthenticateService
    {
        Task<bool> SignInAsync(string login, string password, HttpContext context);
        Task SignOutAsync(HttpContext context);
    }
}
