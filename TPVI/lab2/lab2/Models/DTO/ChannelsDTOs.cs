namespace lab2.Models.DTO
{
    public class ChannelInfoDto
    {
        public Guid Id { get; set; }
        public string Name { get; set; } = "";
        public string State { get; set; } = "ACTIVE"; // ACTIVE | CLOSE
        public string Description { get; set; } = "";
    }

    public class ChannelCreateRequest
    {
        public string Command { get; set; } = "";       // "new"
        public string Name { get; set; } = "";
        public string State { get; set; } = "ACTIVE";   // ACTIVE | CLOSE
        public string Description { get; set; } = "";
    }

    public class ChannelUpdateRequest
    {
        public string Command { get; set; } = "";       // "close" | "open"
        public Guid? Id { get; set; }                   // optional
        public string? State { get; set; }              // ACTIVE | CLOSE
        public string? Reason { get; set; }             // for "close"
    }
    public class ChannelDeleteRequest
    {
        public string Command { get; set; } = "";       // "delete"
        public string? State { get; set; }              // optional (ACTIVE | CLOSE)
    }

    public class QueueCommandRequest
    {
        public string Command { get; set; } = "";       // "enqueue" | "dequeue" | "peek"
        public Guid Id { get; set; }                    // channel id
        public string? Data { get; set; }               // optional JSON payload
    }
    public class QueueResponse
    {
        public Guid Id { get; set; }
        public string? Data { get; set; }
        public string? Error { get; set; }
    }
}
