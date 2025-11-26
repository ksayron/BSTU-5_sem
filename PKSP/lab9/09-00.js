const http = require('http');
const url = require('url');
const fs = require('fs');
const path = require('path');
const querystring = require('querystring');

const PORT = 5000;
const UPLOAD_DIR = path.join(__dirname, 'uploads');
if (!fs.existsSync(UPLOAD_DIR)) fs.mkdirSync(UPLOAD_DIR, { recursive: true });

function collectBody(req, cb) {
  const chunks = [];
  req.on('data', c => chunks.push(c));
  req.on('end', () => cb(null, Buffer.concat(chunks)));
  req.on('error', err => cb(err));
}

function buildJsonResponse(obj) {
  const x = parseFloat(obj.x) || 0;
  const y = parseFloat(obj.y) || 0;
  const s = obj.s || '';
  const m = Array.isArray(obj.m) ? obj.m : [];
  const o = obj.o || {};
  return {
    "__commment": obj.__commment || '',
    "x+y": String(x + y),
    "concat_s+o": `${s}: ${o.surname || ''}, ${o.name || ''}`,
    "m_size": String(m.length)
  };
}

function parseXmlRequest(xmlStr) {
  const reqIdMatch = xmlStr.match(/<request[^>]*id="([^"]+)"/);
  const rid = reqIdMatch ? reqIdMatch[1] : '';
  const xMatches = [...xmlStr.matchAll(/<x\s+[^>]*value="([^"]+)"/g)].map(m=>m[1]);
  const mMatches = [...xmlStr.matchAll(/<m\s+[^>]*value="([^"]+)"/g)].map(m=>m[1]);
  return { rid, xMatches, mMatches };
}

const server = http.createServer((req, res) => {
  const parsed = url.parse(req.url, true);
  const pathname = parsed.pathname;

  if (req.method === 'GET' && pathname === '/task01') {
    res.statusCode = 200;
    res.statusMessage = 'OK-task01';
    res.setHeader('Content-Type', 'text/plain; charset=utf-8');
    res.end('Hello from task01 server');
    return;
  }

  if (req.method === 'GET' && pathname === '/sum') {
    const q = parsed.query;
    const x = parseFloat(q.x);
    const y = parseFloat(q.y);
    if (!isNaN(x) && !isNaN(y)) {
      const out = { sum: x + y, diff: x - y, mul: x * y, div: (y!==0? x/y : null) };
      res.writeHead(200, {'Content-Type':'application/json; charset=utf-8'});
      res.end(JSON.stringify(out));
    } else {
      res.writeHead(400, {'Content-Type':'text/plain; charset=utf-8'});
      res.end('x and y should be numbers');
    }
    return;
  }


  if (req.method === 'POST' && pathname === '/post-form') {
    collectBody(req, (err, bodyBuf) => {
      if (err) { res.writeHead(500); res.end('err'); return; }
      const parsedBody = querystring.parse(bodyBuf.toString());
      res.writeHead(200, {'Content-Type':'application/json; charset=utf-8'});
      res.end(JSON.stringify(parsedBody));
    });
    return;
  }

  if (req.method === 'POST' && pathname === '/json') {
    collectBody(req, (err, bodyBuf) => {
      if (err) { res.writeHead(500); res.end('err'); return; }
      let obj;
      try { obj = JSON.parse(bodyBuf.toString()); } catch(e) { res.writeHead(400); res.end('Invalid JSON'); return; }
      const resp = buildJsonResponse(obj);
      res.writeHead(200, {'Content-Type':'application/json; charset=utf-8'});
      res.end(JSON.stringify(resp));
    });
    return;
  }


  if (req.method === 'POST' && pathname === '/xml') {
    collectBody(req, (err, bodyBuf) => {
      if (err) { res.writeHead(500); res.end('err'); return; }
      const xml = bodyBuf.toString();
      const { rid, xMatches, mMatches } = parseXmlRequest(xml);
      const xSum = xMatches.reduce((a,v)=>a+ (parseFloat(v)||0),0);
      const mConcat = mMatches.join('');
      const responseXml = `<responce id="${String(Number(rid||0)+2)}" request="${rid}">
  <sum element="x" result="${xSum}"/>
  <sum element="m" result="${mConcat}"/>
</responce>`;
      res.writeHead(200, {'Content-Type':'application/xml; charset=utf-8'});
      res.end(responseXml);
    });
    return;
  }

  if (req.method === 'POST' && pathname === '/upload-file') {
    const ctype = req.headers['content-type'] || '';
    const m = ctype.match(/boundary=(.+)$/);
    if (!m) { res.writeHead(400); res.end('No boundary'); return; }
    const boundary = '--' + m[1];
    collectBody(req, (err, bodyBuf) => {
      if (err) { res.writeHead(500); res.end('err'); return; }
      const bodyStr = bodyBuf.toString('binary'); 
      const parts = bodyStr.split(boundary).filter(p=>p && p!=='--\r\n' && p!=='--');
      let savedFiles = [];
      for (const part of parts) {
        const headerEnd = part.indexOf('\r\n\r\n');
        if (headerEnd === -1) continue;
        const headers = part.slice(0, headerEnd);
        const content = part.slice(headerEnd+4, part.length-2); 
        const fnMatch = headers.match(/filename="([^"]+)"/i);
        if (fnMatch) {
          const filename = path.basename(fnMatch[1]);
          const buf = Buffer.from(content, 'binary');
          const outPath = path.join(UPLOAD_DIR, filename);
          fs.writeFileSync(outPath, buf);
          savedFiles.push(filename);
        }
      }
      res.writeHead(200, {'Content-Type':'application/json; charset=utf-8'});
      res.end(JSON.stringify({ saved: savedFiles }));
    });
    return;
  }


  if (req.method === 'GET' && pathname === '/download') {
    const fname = parsed.query.file;
    if (!fname) { res.writeHead(400); res.end('file param required'); return; }
    const fpath = path.join(UPLOAD_DIR, path.basename(fname));
    if (!fs.existsSync(fpath)) { res.writeHead(404); res.end('not found'); return; }
    const stat = fs.statSync(fpath);
    res.writeHead(200, {
      'Content-Type': 'application/octet-stream',
      'Content-Length': stat.size,
      'Content-Disposition': 'attachment; filename="' + path.basename(fpath) + '"'
    });
    fs.createReadStream(fpath).pipe(res);
    return;
  }


  res.writeHead(404, {'Content-Type':'text/plain; charset=utf-8'});
  res.end('Not found');
});

server.listen(PORT, () => console.log(`Test server listening at http://localhost:${PORT}`));