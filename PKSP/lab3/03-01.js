var http = require('http');

const PORT = 5000;

let state = "norm";

http.createServer(function (request, response) {
      response.writeHead("200", { "Content-type": "text/html; charset=utf-8" });
      response.end(`<!DOCTYPE html>
        <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>03-01</title>
            </head>
            <body>
                <h1 id="state-display">${state}</h1>
            </body>
        </html>`);
  })
  .listen(5000, () => {
    console.log("Server running at http://localhost:5000/");
  });


process.stdin.setEncoding("utf-8");
process.stdin.on("readable", () => {
  let stroka = null;
  const states = ["norm", "stop", "idle", "exit"];

  while ((stroka = process.stdin.read()) != null) {
    let trimmedInput = stroka.trim();

    if (states.includes(trimmedInput)) {
      process.stdout.write(`${state} -> ${trimmedInput}\n`);

      if (trimmedInput === "exit") {
        process.exit(0);
      } else {
        state = trimmedInput;
      }
    } else {
      process.stdout.write('Incorrect state: ${trimmedInput}\n');
    }
  }
});