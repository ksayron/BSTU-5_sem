
using ASP_1.Modules;
using BSTU.Results.Authenticate;
using BSTU.Results.Collection;
using Microsoft.AspNetCore.Authentication;
using Microsoft.AspNetCore.Identity;
using Microsoft.EntityFrameworkCore;
using System;

namespace ASP_1
{
    public class Program
    {
        public static async Task Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            builder.Services.AddDbContext<AppDbContext>(options => options.UseInMemoryDatabase("AuthDb"));


            builder.Services.AddIdentity<IdentityUser, IdentityRole>(options =>
            {
                options.Password.RequireDigit = false;
                options.Password.RequireLowercase = false;
                options.Password.RequireUppercase = false;
                options.Password.RequiredLength = 4;
                options.Password.RequireNonAlphanumeric = false;
            }).
            AddEntityFrameworkStores<AppDbContext>()
            .AddDefaultTokenProviders();

            builder.Services.AddScoped<AuthenticationService>();

            builder.Services.AddControllersWithViews();

            builder.Services.AddAuthorization(options =>
            {
                options.AddPolicy("RequireReader", policy => policy.RequireRole("READER"));
                options.AddPolicy("RequireWriter", policy => policy.RequireRole("WRITER"));
            });

            builder.Services.AddEndpointsApiExplorer();
            builder.Services.AddSwaggerGen();


            using WebApplication app = builder.Build();

            app.UseSwagger();
            app.UseSwaggerUI(c =>
            {
                c.SwaggerEndpoint("/swagger/v1/swagger.json", "v1");
            });


            using (var scope = app.Services.CreateScope())
            {
                var services = scope.ServiceProvider;
                var roles = services.GetRequiredService<RoleManager<IdentityRole>>();
                var users = services.GetRequiredService<UserManager<IdentityUser>>();

                if (!await roles.RoleExistsAsync("READER"))
                {
                    IdentityRole role = new IdentityRole("READER");
                    await roles.CreateAsync(role);
                }
                if (!await roles.RoleExistsAsync("WRITER"))
                {
                    IdentityRole role = new IdentityRole("WRITER");
                    await roles.CreateAsync(role);
                }

                var readerUser = await users.FindByNameAsync("reader");
                if (readerUser == null)
                {
                    readerUser = new IdentityUser("reader");
                    await users.CreateAsync(readerUser, "12345");
                    await users.AddToRoleAsync(readerUser, "READER");
                }

                var writerUser = await users.FindByNameAsync("writer");
                if (writerUser == null)
                {
                    writerUser = new IdentityUser("writer");
                    await users.CreateAsync(writerUser, "12345");
                    await users.AddToRoleAsync(writerUser, "WRITER");
                }
            }

            app.UseHttpsRedirection();
            app.UseStaticFiles();

            app.UseRouting();


            app.UseSwagger();
            app.UseSwaggerUI(c =>
            {
                c.SwaggerEndpoint("/swagger/v1/swagger.json", "ResultsAPI v1.0.4");
                c.RoutePrefix = string.Empty;
            });
            app.UseAuthorization();

            app.MapControllerRoute(
                name: "default",
                pattern: "{controller=Home}/{action=Index}/{id?}");

            app.Run();
        }
    }
}
