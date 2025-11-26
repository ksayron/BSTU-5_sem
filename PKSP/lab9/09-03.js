const http = require('http');
const qs = require('querystring');

const postBody = qs.stringify({ x: 10, y: 5, s: 'hello' });
const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/parameter',
  method: 'POST',
  headers: {
    'Content-Type': 'application/x-www-form-urlencoded',
    'Content-Length': Buffer.byteLength(postBody)
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode);
  const chunks = [];
  res.on('data', c=>chunks.push(c));
  res.on('end', ()=> console.log('body:', Buffer.concat(chunks).toString()));
});
req.on('error', console.error);
req.write(postBody);
req.end();