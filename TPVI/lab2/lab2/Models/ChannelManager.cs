using System.Collections.Concurrent;
using System.Threading.Channels;

namespace lab2.Models
{
    public class ChannelManager
    {
        ConcurrentDictionary<Guid, ChannelWrapper> _channels = new();
        ChannelManagerOptions _options;
        ILogger<ChannelManager> _logger;

        public ChannelManager(ILogger<ChannelManager> logger,ChannelManagerOptions options)
        {
            _logger = logger;
            _options = options;
        }
        public IEnumerable<ChannelInfo> GetChannels() => _channels.Values.Select(c => c.Info).ToList();
        public ChannelInfo? GetChannelByGuid(Guid id) => _channels.TryGetValue(id,out var c)? c.Info : null;

        public ChannelInfo CreateChannel(string name, ChannelState state,string? desc=null,int? capacity = null)
        {
            var id = Guid.NewGuid();
            var info = new ChannelInfo
            {
                Id = id,
                Name = name,
                State = state,
                Description = desc ?? string.Empty,
                Capacity = capacity ?? _options.DefaultCapacity
            };
            var wrapper = new ChannelWrapper
            {
                Info = info,
                CapacitySemaphore = new SemaphoreSlim(info.Capacity, info.Capacity)
            };
            _channels[id] = wrapper;
            _logger.LogInformation($"Created channel {name} with {id}");
            return info;
        }
        public IEnumerable<ChannelInfo> ChangeStates(ChannelState state,string? reason = null)
        {
            reason ??= string.Empty;
            foreach(var c  in _channels.Values)
            {
                c.Info.State = state;
                _logger.LogInformation($"Channel {c.Info.Id} changed states. Reason: {reason}");
                
            }
            return GetChannels();
        }
        public ChannelInfo? ChangeStateById(Guid id,ChannelState state,string? reason = null)
        {
            reason ??= string.Empty;

            if (_channels.TryGetValue(id, out var c))
            {
                c.Info.State = state;
                _logger.LogInformation($"Channel {c.Info.Id} changed states. Reason: {reason}");
                return c.Info;
            }
            return null;
        }
        public IEnumerable<ChannelInfo> DeleteAll()
        {
            var before = GetChannels();
            _channels.Clear();
            _logger.LogInformation("All channels deleted");
            return before;
        }

        public IEnumerable<ChannelInfo> DeleteByState(ChannelState state)
        {
            var before = _channels.Values.Select(c=>c.Info).Where(i=>i.State == state).ToList();
            var keysOfRemoved = _channels.Where(kv => kv.Value.Info.State == state).Select(kv => kv.Key).ToList();
            foreach (var id in keysOfRemoved) 
                _channels.TryRemove(id, out _);
            _logger.LogInformation($"Channels with state {state} deleted: {keysOfRemoved.Count}");
            return before;
        }
        public async Task<(bool success, string? error)> EnqueueAsync(Guid channelId, string data)
        {
            if (!_channels.TryGetValue(channelId, out var w)) 
                return (false, "Channel not found");

            if (w.Info.State == ChannelState.CLOSE)
            {
                _logger.LogWarning("Enqueue attempted for closed channel {Id}", channelId);
                return (false, "Channel closed");
            }

            var cts = new CancellationTokenSource(TimeSpan.FromSeconds(_options.WaitEnqueueSeconds));
            try
            {
                await w.CapacitySemaphore.WaitAsync(cts.Token);
                var item = new Item { Id = channelId, JsonString = data };
                w.Queue.Enqueue(item);
                await w.Signal.Writer.WriteAsync(item);
                _logger.LogInformation("Enqueued item to channel {Id}", channelId);
                return (true, null);
            }
            catch (OperationCanceledException)
            {
                _logger.LogWarning("WaitEnqueue expired for channel {Id}", channelId);
                return (false, "WaitEnqueue timeout");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error enqueue to channel {Id}", channelId);
                return (false, ex.Message);
            }
        }
        public async Task<(bool success, Item? item, string? error)> DequeueAsync(Guid channelId, int timeoutSeconds = 0)
        {
            if (!_channels.TryGetValue(channelId, out var w))
                return (false, null, "Channel not found");

            if (w.Queue.TryDequeue(out var item))
            {
                w.CapacitySemaphore.Release();
                _logger.LogInformation("Dequeued existing item from channel {Id}", channelId);
                return (true, item, null);
            }

            try
            {
                var cts = timeoutSeconds > 0
                    ? new CancellationTokenSource(TimeSpan.FromSeconds(timeoutSeconds))
                    : new CancellationTokenSource();

                var readTask = w.Signal.Reader.ReadAsync(cts.Token).AsTask();
                item = await readTask;
                if (w.Queue.TryDequeue(out var queued))
                {
                    w.CapacitySemaphore.Release();
                    return (true, queued, null);
                }
                return (false, null, "No item after signal");
            }
            catch (OperationCanceledException)
            {
                _logger.LogWarning("Dequeue timeout for channel {Id}", channelId);
                return (false, null, "Dequeue timeout");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error during Dequeue on channel {Id}", channelId);
                return (false, null, ex.Message);
            }
        }
        public (bool success, Item? item, string? error) Peek(Guid channelId)
        {
            if (!_channels.TryGetValue(channelId, out var w))
                return (false, null, "Channel not found");
            if (w.Queue.TryPeek(out var item))
            {
                return (true, item, null);
            }
            return (false, null, "No items");
        }
    }
}
