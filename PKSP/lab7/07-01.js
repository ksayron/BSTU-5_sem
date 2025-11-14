const http = require('http');
const fs = require('fs');
const url = require('url');
const path = require('path');
const PORT = 5000;
const { createStaticHandler } = require('./07-01m.js');

const staticHandler = createStaticHandler('static');

const serverFunction = function (request, response) {
    const ext = path.extname(url.parse(request.url).pathname);
    if (ext) {
         staticHandler(request, response);
    }
    else {
        response.writeHead(405, { 'contnt-type': 'text/html;charset=utf-8' });
        response.end("<h1>405 Method not alowed</h1>");
    }
}


const server = http.createServer(serverFunction);

server.listen(PORT);

console.log(`Server running at http://localhost:${PORT}/index.html`);
