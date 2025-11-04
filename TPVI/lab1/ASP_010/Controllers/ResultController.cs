using ASP_010.Models;
using BSTU.Results.Collection;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace ASP_010.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    [Authorize]
    public class ResultsController : ControllerBase
    {
        private readonly IResultsService _svc;
        public ResultsController(IResultsService svc) => _svc = svc;


        // GET /api/Results
        [HttpGet]
        public async Task<IActionResult> GetAll()
        {
            var items = await _svc.GetAllAsync();
            if (items == null || !items.Any()) return NoContent(); // 204
            return Ok(items); // 200
        }


        // GET /api/Results/{k}
        [HttpGet("{k:int}")]
        public async Task<IActionResult> Get(int k)
        {
            var item = await _svc.GetResultByIdAsync(k);
            if (item == null) return NotFound();
            return Ok(item);
        }


        // POST /api/Results
        [HttpPost]
        public async Task<IActionResult> Post([FromBody] ResultDto dto)
        {
            if (dto == null || string.IsNullOrWhiteSpace(dto.Message)) return BadRequest();
            var added = await _svc.AddResultAsync(dto.Message);
            return CreatedAtAction(nameof(Get), new { k = added.Key }, added);
        }


        // PUT /api/Results/{k}
        [HttpPut("{k:int}")]
        public async Task<IActionResult> Put(int k, [FromBody] ResultDto dto)
        {
            if (dto == null || string.IsNullOrWhiteSpace(dto.Message)) return BadRequest();
            var updated = await _svc.UpdateResultAsync(k, dto.Message);
            if (updated == null) return NotFound();
            return Ok(updated);
        }


        // DELETE /api/Results/{k}
        [HttpDelete("{k:int}")]
        public async Task<IActionResult> Delete(int k)
        {
            var ok = await _svc.DeleteResultAsync(k);
            if (!ok) return NotFound();
            return Ok();
        }
    }
}
