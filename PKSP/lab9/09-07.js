// 09-07.js
const http = require('http');
const fs = require('fs');
const path = require('path');

const filePath = path.join(__dirname, 'uploads/pic.png'); 
if (!fs.existsSync(filePath)) {
        console.log(filePath);
  process.exit(1);
}

const boundary = '----NodeMultipartBoundary' + Date.now();
const stat = fs.statSync(filePath);
const filename = path.basename(filePath);
const pre = `--${boundary}\r\nContent-Disposition: form-data; name="file"; filename="${filename}"\r\nContent-Type: image/png\r\n\r\n`;
const post = `\r\n--${boundary}--\r\n`;
const preBuf = Buffer.from(pre, 'utf8');
const postBuf = Buffer.from(post, 'utf8');
const totalLength = preBuf.length + stat.size + postBuf.length;

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/upload',
  method: 'POST',
  headers: {
    'Content-Type': 'multipart/form-data; boundary=' + boundary,
    'Content-Length': totalLength
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode);
  const bufs = [];
  res.on('data', c=>bufs.push(c));
  res.on('end', ()=> console.log('body:', Buffer.concat(bufs).toString()));
});
req.on('error', console.error);

req.write(preBuf);
const rs = fs.createReadStream(filePath);
rs.on('end', ()=> {
  req.write(postBuf);
  req.end();
});
rs.pipe(req, { end: false });
