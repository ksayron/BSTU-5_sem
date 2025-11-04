using ASP_1.Modules;
using BSTU.Results.Authenticate;
using BSTU.Results.Collection;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

namespace ASP_1.Controllers
{
    public record LoginDto(string Login, string Password);

    [ApiController]
    [Route("api/[controller]")]
    [Authorize]
    public class ResultsController : ControllerBase
    {
        private readonly IResultsService _results;
        private readonly IAuthenticateService _auth;
        public ResultsController(IResultsService svc,IAuthenticateService auth)
        {
            svc = new ResultsService();
            _auth = auth;
        }

        [HttpPost("SignIn")]
        public async Task<IActionResult> SignIn([FromBody] LoginDto dto)
        {
            if (string.IsNullOrWhiteSpace(dto.Login) || string.IsNullOrWhiteSpace(dto.Password))
                return BadRequest();

            var ok = await _auth.SignInAsync(dto.Login, dto.Password, HttpContext);
            return ok ? Ok("Authenticated") : NotFound("Invalid credentials");
        }

        [HttpGet("SignOut")]
        [Authorize]
        public async Task<IActionResult> SignOut()
        {
            await _auth.SignOutAsync(HttpContext);
            return Ok("Signed out");
        }


        [HttpGet]
        [Authorize(Roles = "READER,WRITER")]
        public IActionResult GetAll() => Ok(_results.GetAllAsync());

        [HttpGet("{id:int}")]
        [Authorize(Roles = "READER,WRITER")]
        public IActionResult Get(int id)
        {
            var result = _results.GetResultByIdAsync(id);
            return result == null ? NotFound() : Ok(result);
        }

        [HttpPost]
        [Authorize(Roles = "WRITER")]
        public IActionResult Add([FromBody] ResultDto dto)
        {
            var added = _results.AddResultAsync(dto.Message);
            return CreatedAtAction(nameof(Get), new { id = added.Id }, added);
        }

        [HttpPut("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public IActionResult Update(int id, [FromBody] ResultDto dto)
        {
            if (_results.UpdateResultAsync(id, dto.Message) == null)
                return NotFound();
            return Ok(_results.GetResultByIdAsync(id));
        }

        [HttpDelete("{id:int}")]
        [Authorize(Roles = "WRITER")]
        public async Task<IActionResult> Delete(int id)
        {
            if ( await _results.DeleteResultAsync(id) == false)
                return NotFound();
            return Ok();
        }
    }
}
