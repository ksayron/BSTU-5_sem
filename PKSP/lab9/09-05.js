const http = require('http');

const xml = `<?xml version="1.0"?>
<request id="28">
  <x value="7"/>
  <x value="2"/>
  <x value="3"/>
  <m value="a"/>
  <m value="b"/>
  <m value="c"/>
</request>`;

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/xml',
  method: 'POST',
  headers: {
    'Content-Type': 'application/xml',
    'Content-Length': Buffer.byteLength(xml)
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode, res.statusMessage);
  const bufs = [];
  res.on('data', c=>bufs.push(c));
  res.on('end', ()=> {
    const body = Buffer.concat(bufs).toString();
    console.log('body:', body);
  });
});
req.on('error', console.error);
req.write(xml);
req.end();
