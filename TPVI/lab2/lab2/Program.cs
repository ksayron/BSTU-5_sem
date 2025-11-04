
using lab2.Models;
using Serilog;

namespace lab2
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var builder = WebApplication.CreateBuilder(args);

            Log.Logger = new LoggerConfiguration()
                .ReadFrom.Configuration(builder.Configuration)
                .CreateLogger();

            builder.Host.UseSerilog();
            builder.Services.AddControllers();
            builder.Services.AddEndpointsApiExplorer();
            builder.Services.AddSwaggerGen();

            var waitEnqueue = builder.Configuration.GetValue<int>("WaitEnqueue", 5);
            var defaultCapacity = builder.Configuration.GetValue<int>("DefaultChannelCapacity", 100);

            builder.Services.AddSingleton(new ChannelManagerOptions
            {
                DefaultCapacity = defaultCapacity,
                WaitEnqueueSeconds = waitEnqueue
            });
            builder.Services.AddSingleton<ChannelManager>();

            var app = builder.Build();

            // Configure the HTTP request pipeline.
            if (app.Environment.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
                app.UseSwagger();
                app.UseSwaggerUI();
            }

            app.UseSerilogRequestLogging();
            app.UseHttpsRedirection();

            app.UseAuthorization();


            app.MapControllers();

            app.Run();
        }
    }
}
