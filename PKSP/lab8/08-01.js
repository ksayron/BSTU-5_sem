// server.js
const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');
const querystring = require('querystring');
const xml2js = require("xml2js");
const xmlBuilder = require("xmlbuilder");
const mp = require("multiparty");


const PORT = 5000;
const STATIC_DIR = path.resolve(process.cwd(), 'static');
let reqCount = 0;

const MIME = {
  html: 'text/html',
  css: 'text/css',
  js: 'text/javascript',
  png: 'image/png',
  docx: 'application/msword',
  json: 'application/json',
  xml: 'application/xml',
  mp4: 'video/mp4'
};

function safeJoin(base, p) {
  const normal = path.normalize(path.join(base, p));
  if (!normal.startsWith(base)) return null;
  return normal;
}

function parsePathParts(reqUrl) {
  const parsed = url.parse(reqUrl, true);
  return { pathname: parsed.pathname, query: parsed.query };
}

function send404(res) {
  res.writeHead(404, { 'Content-Type': 'text/html; charset=utf-8' });
  res.end('<h1>404 Not Found</h1>');
}

function send405(res) {
  res.writeHead(405, { 'Content-Type': 'text/plain; charset=utf-8' });
  res.end('405 Method Not Allowed');
}

function collectBody(req, onComplete) {
  let body = '';
  req.on('data', chunk => {
    body += chunk.toString();
  });
  req.on('end', () => onComplete(null, body));
  req.on('error', err => onComplete(err));
}

const server = http.createServer(async (req, res) => {
  reqCount++;
  const { pathname, query } = parsePathParts(req.url);

  
  if (req.method === 'GET' && pathname === '/connection') {
    if (query && query.set !== undefined) {
      const newVal = parseInt(query.set, 10);
      if (!isNaN(newVal) && newVal >= 0) {
        server.keepAliveTimeout = newVal;
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(`Установлено новое значение KeepAliveTimeout=${newVal}`);ф
      } else {
        res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end('Неверное значение set');
      }
    } else {
      res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end(`KeepAliveTimeout=${server.keepAliveTimeout}`);
    }
    return;
  }

  if (req.method === 'GET' && pathname === '/headers') {
    const reqHeaders = req.headers;
    res.setHeader('X-KNP-Header', 'My-Header');

    const responseHeaderNames = res.getHeaderNames ? res.getHeaderNames() : Object.keys(res._headers || {});
    let html = '<h2>Request headers</h2><p>' + JSON.stringify(reqHeaders, null, 2) + '</p>';
    html += '<h2>Response headers (sent)</h2><p>' + JSON.stringify(
      responseHeaderNames.reduce((acc, k) => { acc[k] = res.getHeader(k); return acc; }, {}), null, 2
    ) + '</p>';
    html += '<h3>Пояснения</h3><ul>';
    html += '<li><b>Host</b> — имя/порт сервера, к которому клиент подключился.</li>';
    html += '<li><b>User-Agent</b> — информация о клиенте (браузере или инструменте).</li>';
    html += '<li><b>Accept</b> — какие типы контента клиент готов принять.</li>';
    html += '<li><b>X-My-Header</b> — пользовательский заголовок ответа (демонстрация).</li>';
    html += '</ul>';
    res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
    res.end(html);
    return;
  }

  if (req.method === 'GET' && pathname === '/parameter' && ('x' in query || 'y' in query)) {
    const x = query.x;
    const y = query.y;
    const nx = parseFloat(x);
    const ny = parseFloat(y);
    if (!isNaN(nx) && !isNaN(ny)) {
      const out = {
        sum: nx + ny,
        diff: nx - ny,
        mul: nx * ny,
        div: ny !== 0 ? nx / ny : 'Division by zero'
      };
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(out));
    } else {
      res.writeHead(400, { 'Content-Type': 'text/plain; charset=utf-8' });
      res.end('wrong parameter format');
    }
    return;
  }
  
 if (req.method === 'POST' && pathname === '/parameter') {
      collectBody(req, (err, bodyBuf) => {
        if (err) { res.writeHead(500); res.end('err'); return; }
        const parsedBody = querystring.parse(bodyBuf.toString());
        res.writeHead(200, {'Content-Type':'application/json; charset=utf-8'});
        res.end(JSON.stringify(parsedBody));
      });
    return;
  }
  

  if (req.method === 'GET' && pathname.startsWith('/parameter/')) {
    const parts = pathname.split('/').filter(Boolean);
    if (parts.length >= 3) {
      const x = parts[1];
      const y = parts[2];
      const nx = parseFloat(x);
      const ny = parseFloat(y);
      if (!isNaN(nx) && !isNaN(ny)) {
        const html = `
          <p>sum: ${nx + ny}</p>
          <p>diff: ${nx - ny}</p>
          <p>mul: ${nx * ny}</p>
          <p>div: ${ny !== 0 ? nx / ny : 'Division by zero'}</p>
        `;
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(html);
      } else {
        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.end(`URI: ${pathname}`);
      }
      return;
    }
  }

  if (req.method === 'GET' && pathname === '/close') {
    res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end('Server shutdown in 10 seconds');
    console.log('Server shutdown in 10 seconds');
    setTimeout(() => {
      server.close(() => {
        console.log('Server closed');
        process.exit(0);
      });
    }, 10000);
    return;
  }

  if (req.method === 'GET' && pathname === '/socket') {
    const sock = req.socket;
    const clientIp = sock.remoteAddress;
    const clientPort = sock.remotePort;
    const serverIp = sock.localAddress;
    const serverPort = sock.localPort;
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify({
      clientIp, clientPort, serverIp, serverPort
    }, null, 2));
    return;
  }

  if ((req.method === 'POST' || req.method === 'GET') && pathname === '/req-data') {
    if (req.method === 'GET') {
      const html = `
        <form method="POST" action="/req-data">
          <textarea name="data" rows="15" cols="80"></textarea><br>
          <button type="submit">Send</button>
        </form>
      `;
      res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
      res.end(html);
      return;
    } else {
      let bytes = 0;
      let chunks = 0;
      req.on('data', (chunk) => {
        chunks++;
        bytes += chunk.length;
      });
      req.on('end', () => {
        res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ chunks, bytes }));
      });
      req.on('error', () => send405(res));
      return;
    }
  }

  if (req.method === 'GET' && pathname === '/resp-status') {
    const code = parseInt(query.code, 10) || 200;
    const mess = query.mess || '';
    res.statusMessage = mess;
    res.writeHead(code, { 'Content-Type': 'text/plain; charset=utf-8' });
    res.end(`Status set to ${code} ${mess}`);
    return;
  }

  if (pathname === '/formparameter') {
    if (req.method === 'GET') {
      const html = `
        <form method="POST" action="/formparameter">
          <input type="text" name="text" placeholder="text"><br>
          <input type="number" name="num" placeholder="number"><br>
          <input type="date" name="date"><br>
          <label><input type="checkbox" name="cb" value="on">checkbox</label><br>
          <label><input type="radio" name="rb" value="r1">r1</label>
          <label><input type="radio" name="rb" value="r2">r2</label><br>
          <textarea name="text"></textarea><br>
          <input type="submit" name="btn" value="send">
          <input type="submit" name="btn" value="send2">
        </form>
      `;
      res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
      res.end(html);
      return;
    } else if (req.method === 'POST') {
      collectBody(req, (err, body) => {
        if (err) { res.writeHead(500); res.end('error'); return; }
        const parsed = querystring.parse(body);
        res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify(parsed, null, 2));
      });
      return;
    } else {
      send405(res); return;
    }
  }

  if (req.method === 'POST' && pathname === '/json') {
    collectBody(req, (err, body) => {
      if (err) { res.writeHead(500); res.end('error'); return; }
      let obj;
      try { obj = JSON.parse(body); } catch (e) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 'Invalid JSON' })); return;
      }
      const x = parseFloat(obj.x) || 0;
      const y = parseFloat(obj.y) || 0;
      const s = obj.s || '';
      const m = Array.isArray(obj.m) ? obj.m : [];
      const o = obj.o || {};
      const resp = {
        "__commment": obj.__commment || '',
        "x+y": (x + y).toString(),
        "concat_s+o": `${s}: ${o.surname || ''}, ${o.name || ''}`,
        "m_size": m.length.toString()
      };
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(resp, null, 2));
    });
    return;
  }

  if (req.method === 'POST' && pathname === '/xml') {
     collectBody(req, (err, body) => {
    if (err) {
      res.writeHead(400, { "Content-Type": "text/plain; charset=utf-8" });
      res.end("Error reading request body");
      return;
    }

    const parser = new xml2js.Parser({ explicitArray: true, explicitCharkey: false });
    parser.parseString(body, (err, result) => {
      if (err) {
        res.writeHead(400, { "Content-Type": "text/plain; charset=utf-8" });
        res.end("Invalid XML format");
        return;
      }

      const request = result.request;
      const id = parseInt(request.$.id);
      const xs = request.x || [];
      const ms = request.m || [];

      let sum = 0;
      let mess = "";

      xs.forEach(elem => {
        sum += parseInt(elem.$.value);
      });

      ms.forEach(elem => {
        mess += elem.$.value;
      });

      const responseXml = xmlBuilder
        .create("response")
        .att("id", id + xs.length + ms.length)
        .att("request", id)
        .ele("sum", { element: "x", result: `${sum}` }).up()
        .ele("concat", { element: "m", result: `${mess}` })
        .end({ pretty: true });

      res.writeHead(200, { "Content-Type": "text/xml; charset=utf-8" });
      res.end(responseXml);
    });
  });
  return;
  }

  if (req.method === 'GET' && pathname === '/files') {
    fs.readdir(STATIC_DIR, { withFileTypes: true }, (err, items) => {
      if (err) { res.writeHead(500); res.end('error'); return; }
      const count = items.filter(it => it.isFile()).length;
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8', 'X-static-files-count': String(count) });
      res.end(JSON.stringify({ count }));
    });
    return;
  }

  if (req.method === 'GET' && pathname.startsWith('/files/')) {
    const fname = pathname.substring('/files/'.length); 
    const abs = safeJoin(STATIC_DIR, fname);
    if (!abs) { send404(res); return; }
    fs.stat(abs, (err, st) => {
      if (err || !st.isFile()) { send404(res); return; }
      const ext = path.extname(abs).replace('.', '').toLowerCase();
      const contentType = MIME[ext] || 'application/octet-stream';
      res.writeHead(200, { 'Content-Type': contentType, 'Content-Length': st.size });
      const stream = fs.createReadStream(abs);
      stream.on('error', () => { res.writeHead(500); res.end('error'); });
      stream.pipe(res);
    });
    return;
  }

  if (pathname === '/upload') {
    if (req.method === 'GET') {
      const html = `
      <form method="POST" action="/upload" enctype="multipart/form-data">
        <input type="file" name="file"><br><br>
        <input type="submit" value="Upload">
      </form>`;
      res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
      res.end(html);
      return;
    } else if (req.method === 'POST') {
      const form = new mp.Form({ uploadDir: './static' });

    form.on('file', (name, file) => {
      console.log(`filename: ${name} = ${file.originalFilename} in ${file.path}`);
    });

    form.on('error', err => {
      if (!res.writableEnded) {
        res.writeHead(500, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end(`<p>form returned error: ${err}</p>`);
      }
    });

    form.on('close', () => {
      if (!res.writableEnded) {
        res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
        res.end('<p>File uploaded</p>');
      }
    });

    form.parse(req);
    return;
    } else {
      send405(res); return;
    }
  }
  if (pathname === '/multipart') {
    if (req.method === 'GET') {
      const html = `
      <form method="POST" action="/multipart" enctype="multipart/form-data">
        <label>Username: <input type="text" name="text" value="first"></label><br><br>
        <label>Age: <input type="number" name="number" value="100"></label><br><br>
        <label>Is student: <input type="checkbox" name="checkbox" checked></label><br><br>
        <label>Avatar: <input type="file" name="pic"></label><br><br>
        <input type="submit" value="Send">
      </form>
      `;
      res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
      res.end(html);
    }

    else if (req.method === 'POST') {
      let data = Buffer.alloc(0);

      // Получаем тело запроса "как есть"
      req.on('data', chunk => {
        data = Buffer.concat([data, chunk]);
      });

      req.on('end', () => {
        const rawBody = data.toString('binary');

        const contentType = req.headers['content-type'] || '';
        const match = contentType.match(/boundary=(.*)$/);
        const boundary = match ? match[1] : '(no boundary found)';

        res.writeHead(200, { 'Content-Type': 'text/plain; charset=utf-8' });
        res.write(`Content-Type: ${contentType}\nBoundary: ${boundary}\n\n`);
        res.end(rawBody);
      });

      return;
    }

    else {
      res.end('Method not allowed');
    }
  } else {
    res.end('Not found');
  }


});

server.listen(PORT, () => {
  console.log(`Server listening on http://localhost:${PORT}`);
});
server.on("connection", (socket) => {
  console.log(`server.on connection ${socket.remoteAddress}:${socket.remotePort}`);
});
