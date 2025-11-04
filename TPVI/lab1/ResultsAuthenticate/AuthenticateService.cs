using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BSTU.Results.Authenticate
{
    public class AuthenticateService : IAuthenticateService
    {
        private readonly UserManager<IdentityUser> _userManager;
        private readonly SignInManager<IdentityUser> _signInManager;

        public AuthenticateService(UserManager<IdentityUser> userManager,
                                   SignInManager<IdentityUser> signInManager)
        {
            _userManager = userManager;
            _signInManager = signInManager;
        }

        public async Task<bool> SignInAsync(string login, string password, HttpContext context)
        {
            var user = await _userManager.FindByNameAsync(login);
            if (user == null) return false;

            var result = await _signInManager.PasswordSignInAsync(user, password, true, false);
            return result.Succeeded;
        }

        public async Task SignOutAsync(HttpContext context)
        {
            await _signInManager.SignOutAsync();
        }

        public static void AddAuthenticateService(IServiceCollection services, string connection)
        {
            services.AddDbContext<AuthDbContext>(opt => opt.UseInMemoryDatabase("AuthDB"));

            services.AddIdentity<IdentityUser, IdentityRole>()
                .AddEntityFrameworkStores<AuthDbContext>()
                .AddDefaultTokenProviders();

            services.ConfigureApplicationCookie(opt =>
            {
                opt.LoginPath = "/api/Results/SignIn";
                opt.AccessDeniedPath = "/api/Results/Forbidden";
            });

            services.AddScoped<IAuthenticateService, AuthenticateService>();
        }
    }

    public class AuthDbContext : IdentityDbContext<IdentityUser>
    {
        public AuthDbContext(DbContextOptions<AuthDbContext> options) : base(options) { }
    }
}
