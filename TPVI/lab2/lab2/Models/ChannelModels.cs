using System;
using System.Collections.Concurrent;
using System.Threading.Channels;
namespace lab2.Models
{
    public enum ChannelState { ACTIVE, CLOSE }

    public record ChannelInfo
    {
        public Guid Id { get; init; }
        public string Name { get; init; } = string.Empty;
        public ChannelState State { get; set; }
        public string Description { get; init; } = string.Empty;
        public int Capacity { get; init; } = 100;
        public DateTime CreatedAt { get; init; } = DateTime.UtcNow;
    }

    public class ChannelWrapper
    {
        public ChannelInfo Info { get; init; } = default!;
        public ConcurrentQueue<Item> Queue { get; } = new();
        public Channel<Item> Signal { get; } = Channel.CreateUnbounded<Item>(new UnboundedChannelOptions { SingleReader = false, SingleWriter = false });
        public SemaphoreSlim CapacitySemaphore { get; init; } = new(1, int.MaxValue);
    }

    public class ChannelManagerOptions
    {
        public int WaitEnqueueSeconds { get; set; } = 5;
        public int DefaultCapacity { get; set; } = 100;
    }
}
