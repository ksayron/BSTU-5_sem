using lab2.Models;
using lab2.Models.DTO;
using Microsoft.AspNetCore.Mvc;
using System.Text.Json;
using System.Threading.Channels;

namespace lab2.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class ChannelsController : ControllerBase
    {
        ChannelManager _manager;
        ILogger<ChannelsController> _logger;

        public ChannelsController(ILogger<ChannelsController> logger, ChannelManager manager)
        {
            _logger = logger;
            _manager = manager;
        }
        //<summmary>
        //GET /api/channels
        //</summary>
        [HttpGet]
        public IActionResult GetChannels()
        {
            var all = _manager.GetChannels().ToList();
            if (all.Count == 0) 
                return NoContent();
            return Ok(all);
        }

        //<summmary>
        //GET /api/channels/(guid)
        //</summary>
        [HttpGet("{id:guid}")]
        public IActionResult Get(Guid id)
        {
            var info = _manager.GetChannelByGuid(id);
            if (info == null) 
                return NotFound();
            return Ok(info);

        }

        //<summmary>
        // POST /api/channels
        //</summary>
        [HttpPost]
        public IActionResult Create([FromBody] ChannelCreateRequest body)
        {
            try
            {
                if (body.Command?.ToLower() != "new")
                    return BadRequest("Wrong command");

                var state = Enum.TryParse<ChannelState>(body.State, true, out var parsed)
                    ? parsed : ChannelState.ACTIVE;

                var info = _manager.CreateChannel(body.Name, state, body.Description);
                return info.State == ChannelState.ACTIVE ? Created("", info) : NoContent();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Create channel failed");
                return BadRequest(ex.Message);
            }
        }

        //<summmary>
        // PUT /api/channels
        //</summary>
        [HttpPut]
        public IActionResult UpdateChannel([FromBody] ChannelUpdateRequest body)
        {
            try
            {
                switch (body.Command?.ToLower())
                {
                    case "close":
                        if (body.Id.HasValue)
                            _manager.ChangeStateById(body.Id.Value,ChannelState.CLOSE,body.Reason ?? "");
                        else
                            _manager.ChangeStates(ChannelState.CLOSE,body.Reason ?? "");
                        return Ok("Channel(s) closed");

                    case "open":
                        if (body.Id.HasValue)
                            _manager.ChangeStateById(body.Id.Value, ChannelState.ACTIVE);
                        else
                            _manager.ChangeStateById(body.Id.Value, ChannelState.ACTIVE);
                        return Ok("Channel(s) opened");

                    default:
                        return BadRequest("Wrong command");
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Update channel failed");
                return BadRequest(ex.Message);
            }
        }

        //<summmary>
        // DELETE /api/channels
        //</summary>
        [HttpDelete]
        public IActionResult DeleteChannels([FromBody] ChannelDeleteRequest body)
        {
            try
            {
                if (body.Command?.ToLower() != "delete")
                    return BadRequest("Unsupported command");

                if (!string.IsNullOrEmpty(body.State))
                {
                    var state = Enum.TryParse<ChannelState>(body.State, true, out var s)
                        ? s : ChannelState.ACTIVE;
                    _manager.DeleteByState(state);
                }
                else
                {
                    _manager.DeleteAll();
                }

                return Ok("Channels deleted");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Delete channels failed");
                return BadRequest(ex.Message);
            }
        }


        //<summmary>
        // POST /api/queue
        //</summary>
        [HttpPost("/api/queue")]
        public async Task<IActionResult> QueueOperation([FromBody] QueueCommandRequest request)
        {
            string command = request.Command.ToLowerInvariant();

            try
            {
                switch (command)
                {
                    case "peek":
                        {
                            var peekResult = _manager.Peek(request.Id);
                            if (!peekResult.success)
                            {
                                _logger.LogWarning("Peek failed for {Id}: {Error}", request.Id, peekResult.error);
                                return NotFound(new  { Id = request.Id, Error = peekResult.error });
                            }

                            return Ok(new { Id = request.Id, Data = peekResult.item });
                        }

                    case "dequeue":
                        {
                            var result = await _manager.DequeueAsync(request.Id, 5);

                            if (!result.success)
                            {
                                _logger.LogWarning("Dequeue failed for {Id}: {Error}", request.Id, result.error);
                                return NotFound(new { Id = request.Id, Error = result.error });
                            }

                            return Ok(new { Id = request.Id, Data = result.item });
                        }

                    case "enqueue":
                        {
                            if (string.IsNullOrEmpty(request.Data))
                                return BadRequest("Data is required for enqueue command");

                            var result = await _manager.EnqueueAsync(request.Id, request.Data);

                            if (!result.success)
                            {
                                _logger.LogWarning("Enqueue failed for {Id}: {Error}", request.Id, result.error);
                                return BadRequest(new { Id = request.Id, Error = result.error });
                            }

                            _logger.LogInformation("Successfully enqueued item for {Id}", request.Id);
                            return Ok(new { message = "enqueued", id = request.Id });
                        }

                    default:
                        _logger.LogWarning("Unknown queue command received: {Command}", command);
                        return BadRequest("Unknown queue command");
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Queue operation '{Command}' failed for {Id}", command, request.Id);
                return StatusCode(500, new { Id = request.Id, Error = "Internal server error" });
            }
        }
    }    
}

