const http = require('http');


const server = http.createServer(function (request, response) {
    response.setHeader("Content-Type", "text/html");
    const headers = Object.entries(request.headers)
        .map(([key, value]) => `<li><b>${key}</b>: ${value}</li>`)
        .join('');

    let body = '';

    request.on('data', chunk => {
        body += chunk.toString();
    });

    request.on('end', () => {
        response.end(`<html>
        <head>
            <title>Request info</title>
        </head>
        <body>
            <h1>Request data</h1>
            <p><strong>Method:</strong> ${request.method}</p>
            <p><strong>URI:</strong> ${request.url}</p>
            <h2>Headers:</h2>
            <ul>
                ${headers}
            </ul>
            <h2>Body</h2>
            <p>${body}</p>
        </body>
    </html>`);
    });
}).listen(3000);
console.log("Server active: http://localhost:3000/") 