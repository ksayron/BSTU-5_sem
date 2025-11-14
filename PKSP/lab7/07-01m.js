const http = require('http');
const fs = require('fs');
var url = require('url');
const path = require('path');


const MIME = {
    html: 'text/html;charset=utf-8',
    css: 'text/css',
    js: 'text/javascript',
    png: 'image/png',
    docx: 'application/msword',
    json: 'application/json;charset=utf-8',
    xml: 'application/xml',
    mp4: 'video/mp4'
};


function createStaticHandler(staticRootRelative){
  const staticRoot = path.resolve(process.cwd(), staticRootRelative);

  return function staticHandler(request, response) {

    const parsed = url.parse(request.url);
    let pathname = decodeURIComponent(parsed.pathname);
    const requestedPath = path.normalize(path.join(staticRoot, pathname));

    if(url.parse(request.url).pathname==="/"){
      pathname = '/index.html';
    }
    
    const ext = (path.extname(requestedPath) || '').toLowerCase().replace('.', '');
    if (!ext) {
      response.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
      response.end('<h1>Method is not static</h1>');
      console.log(2);
      return;
    }
    if (request.method !== 'GET') {
      response.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
      response.end('405 Method Not Allowed');
      return;
    }

    

 

    console.log(requestedPath);
    if (!requestedPath.startsWith(staticRoot)) {
      response.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
      response.end('<h1>Wrong static root directory</h1>');
      console.log(1);
      return;
    }
    
    
    if (!Object.prototype.hasOwnProperty.call(MIME, ext)) {
      response.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
      response.end('<h1>Extinsion not supported</h1>');
      console.log(2);
      return;
    }
    fs.stat(requestedPath, (err, stats) => {
      if (err || !stats.isFile()) {
        response.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
        response.end('<h1>File not found</h1>');
        console.log(3);
        return;
      }

      const headers = {
        'Content-Type': MIME[ext] + (ext === 'html' ? '; charset=utf-8' : ''),
        'Content-Length': stats.size
      };

      response.writeHead(200, headers);

      const stream = fs.createReadStream(requestedPath);
      stream.on('error', () => {
        response.writeHead(500, { 'Content-Type': 'text/html; charset=utf-8' });
        response.end('<h1>500 Internal Server Error</h1>');
      });
      stream.pipe(response);
    });
  };
}


module.exports = { createStaticHandler };